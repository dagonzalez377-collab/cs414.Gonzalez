(* An immutable key-value store backed by a persistent (immutable) map.
   Every operation below that manipulates the map itself is a pure
   function: given a store, it returns a new store (or a value) without
   mutating anything. SAVE and LOAD are the exception -- they perform
   file I/O and are therefore impure; that is noted on each one. *)

module StringMap = Map.Make (String)

type store = string StringMap.t

let empty : store = StringMap.empty

(* Pure: produces a new store, the argument store is untouched. *)
let set (key : string) (value : string) (store : store) : store =
  StringMap.add key value store

(* Pure: produces a new store, the argument store is untouched. *)
let delete (key : string) (store : store) : store =
  StringMap.remove key store

(* Pure: just reads, no new store is produced. *)
let get (key : string) (store : store) : string option =
  StringMap.find_opt key store

(* Pure: StringMap.bindings returns entries in ascending key order. *)
let list (store : store) : (string * string) list = StringMap.bindings store

(* Impure: performs file I/O (a side effect). File format is one entry
   per two lines: the key, then the value. *)
let save (filename : string) (store : store) : unit =
  let oc = open_out filename in
  StringMap.iter (fun key value -> Printf.fprintf oc "%s\n%s\n" key value) store;
  close_out oc

(* Impure: performs file I/O (a side effect) and interacts with the
   external environment. Returns a brand new store rather than mutating
   anything -- there is nothing to mutate, since OCaml stores are
   immutable values. *)
let load (filename : string) : store =
  let ic = open_in filename in
  let rec read_pairs acc =
    match input_line ic with
    | exception End_of_file -> acc
    | key ->
      let value =
        try input_line ic
        with End_of_file -> failwith ("malformed save file: " ^ filename)
      in
      read_pairs (StringMap.add key value acc)
  in
  let result = read_pairs StringMap.empty in
  close_in ic;
  result

(* A small transaction helper, matching the pattern described in the
   assignment: apply an operation that may fail; if it succeeds, use
   the new store it produced, otherwise keep using the original store.
   Because stores are immutable values, "rollback" never has to restore
   anything -- the original store was never touched, so simply
   continuing to use it *is* the rollback. *)
let transaction (operation : store -> (store, string) result) (store : store) : store =
  match operation store with
  | Ok new_store -> new_store
  | Error _ -> store
