(* The command loop: this is where all of the program's console I/O and
   mutable control flow lives. The actual key-value logic in Kvstore.Store
   stays pure; this file is the "impure shell" around that pure core. *)

open Kvstore

(* REPL state: the current store, plus (when a transaction is open) the
   store as it was when BEGIN was issued. Because stores are immutable,
   "starting a transaction" just means remembering a reference to the
   store value that existed at that point -- no copying is needed. *)
type state = {
  store : Store.store;
  txn_origin : Store.store option;
}

let print_value = function
  | Some v -> print_endline v
  | None -> print_endline "(nil)"

let print_list store =
  List.iter (fun (k, v) -> Printf.printf "%s = %s\n" k v) (Store.list store)

(* Splits "key value..." into a key and the (possibly multi-word, possibly
   empty) remainder. *)
let split_key_value (args : string) : string * string =
  match String.index_opt args ' ' with
  | None -> (args, "")
  | Some i ->
    (String.sub args 0 i, String.sub args (i + 1) (String.length args - i - 1))

let rec loop (state : state) : unit =
  print_string "> ";
  flush stdout;
  match input_line stdin with
  | exception End_of_file -> ()
  | line -> (
    let line = String.trim line in
    if line = "" then loop state
    else
      let command, args =
        match String.index_opt line ' ' with
        | None -> (line, "")
        | Some i ->
          ( String.sub line 0 i,
            String.trim (String.sub line (i + 1) (String.length line - i - 1)) )
      in
      match command with
      | "SET" ->
        let key, value = split_key_value args in
        if key = "" then (
          print_endline "ERROR: SET requires a key and a value";
          loop state)
        else loop { state with store = Store.set key value state.store }
      | "GET" ->
        print_value (Store.get args state.store);
        loop state
      | "DELETE" -> loop { state with store = Store.delete args state.store }
      | "LIST" ->
        print_list state.store;
        loop state
      | "SAVE" ->
        (try Store.save args state.store
         with Sys_error msg -> Printf.printf "ERROR: %s\n" msg);
        loop state
      | "LOAD" -> (
        match Store.load args with
        | new_store -> loop { state with store = new_store }
        | exception Sys_error msg ->
          Printf.printf "ERROR: %s\n" msg;
          loop state)
      | "BEGIN" ->
        if Option.is_some state.txn_origin then (
          print_endline "ERROR: a transaction is already in progress";
          loop state)
        else loop { state with txn_origin = Some state.store }
      | "COMMIT" ->
        if Option.is_none state.txn_origin then (
          print_endline "ERROR: no transaction in progress";
          loop state)
        else
          (* The changes made since BEGIN are already reflected in
             state.store; committing just means we stop tracking the
             rollback point. *)
          loop { state with txn_origin = None }
      | "ABORT" -> (
        match state.txn_origin with
        | None ->
          print_endline "ERROR: no transaction in progress";
          loop state
        | Some original ->
          (* Roll back by simply resuming use of the untouched original
             store value -- nothing needs to be restored, because the
             original was never mutated. *)
          loop { store = original; txn_origin = None })
      | "QUIT" -> ()
      | other ->
        Printf.printf "ERROR: unknown command '%s'\n" other;
        loop state)

let () = loop { store = Store.empty; txn_origin = None }
