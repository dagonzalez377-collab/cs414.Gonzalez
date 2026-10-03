open Kvstore

let failures = ref 0

let check condition message =
  if condition then Printf.printf "PASS: %s\n" message
  else (
    Printf.eprintf "FAIL: %s\n" message;
    incr failures)

let test_set_and_get () =
  let store = Store.set "name" "Ada" Store.empty in
  check (Store.get "name" store = Some "Ada") "SET then GET returns the value";
  check (Store.get "nope" store = None) "GET on a missing key returns nothing"

let test_set_is_pure () =
  let original = Store.set "name" "Ada" Store.empty in
  let _updated = Store.set "name" "Changed" original in
  check (Store.get "name" original = Some "Ada")
    "SET does not mutate the store passed to it (purity)"

let test_overwrite () =
  let store = Store.empty |> Store.set "x" "1" |> Store.set "x" "2" in
  check (Store.get "x" store = Some "2") "SET overwrites an existing key"

let test_delete () =
  let store = Store.set "name" "Ada" Store.empty in
  let after_delete = Store.delete "name" store in
  check (Store.get "name" after_delete = None) "DELETE removes the key";
  check (Store.get "name" store = Some "Ada")
    "DELETE does not mutate the store passed to it (purity)"

let test_list_sorted () =
  let store =
    Store.empty |> Store.set "name" "Ada" |> Store.set "language" "OCaml"
  in
  let entries = Store.list store in
  check (List.length entries = 2) "LIST returns all entries";
  check
    (entries = [ ("language", "OCaml"); ("name", "Ada") ])
    "LIST returns entries in sorted key order"

let test_save_and_load_round_trip () =
  let store =
    Store.empty |> Store.set "name" "Ada" |> Store.set "language" "OCaml"
  in
  let filename = "/tmp/kvstore_ocaml_test_data.txt" in
  Store.save filename store;
  let loaded = Store.load filename in
  check (Store.get "name" loaded = Some "Ada") "LOAD restores a saved value (name)";
  check
    (Store.get "language" loaded = Some "OCaml")
    "LOAD restores a saved value (language)";
  check (List.length (Store.list loaded) = 2)
    "LOAD restores the correct number of entries";
  Sys.remove filename

let test_transaction_commit () =
  let store = Store.set "x" "10" Store.empty in
  let result =
    Store.transaction
      (fun s -> Ok (Store.set "x" "20" s))
      store
  in
  check (Store.get "x" result = Some "20") "Committed transaction keeps its changes"

let test_transaction_abort_restores_state () =
  (* This mirrors the assignment's required transaction scenario:
       Initial state: x = 10
       Begin transaction
       SET x 20
       SET y 30
       Abort transaction
       Expected final state: x = 10, y does not exist *)
  let original = Store.set "x" "10" Store.empty in
  let result =
    Store.transaction
      (fun s ->
        let s = Store.set "x" "20" s in
        let _s = Store.set "y" "30" s in
        Error "abort")
      original
  in
  check (Store.get "x" result = Some "10")
    "Aborted transaction restores x to its original value";
  check (Store.get "y" result = None)
    "Aborted transaction removes keys introduced during the transaction";
  check (result == original)
    "Aborted transaction returns the exact original store (no copy needed)"

let () =
  test_set_and_get ();
  test_set_is_pure ();
  test_overwrite ();
  test_delete ();
  test_list_sorted ();
  test_save_and_load_round_trip ();
  test_transaction_commit ();
  test_transaction_abort_restores_state ();
  if !failures = 0 then print_endline "\nAll tests passed."
  else (
    Printf.eprintf "\n%d test(s) failed.\n" !failures;
    exit 1)
