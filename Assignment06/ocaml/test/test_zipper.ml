open Zipper

let failures = ref 0

let check condition message =
  if condition then Printf.printf "PASS: %s\n" message
  else (
    Printf.eprintf "FAIL: %s\n" message;
    incr failures)

let () =
  let z = of_list [ 1; 2; 3; 4; 5 ] in
  check (not (is_empty z)) "is_empty false on a non-empty zipper";
  check (is_empty empty) "is_empty true on empty";
  check (focus z = 1) "focus starts at first element";

  let z = move_right z in
  let z = move_right z in
  check (focus z = 3) "move_right x2 lands on 3";

  let z = move_left z in
  check (focus z = 2) "move_left moves back to 2";

  let z = push_front 0 z in
  let z = push_back 6 z in
  check (to_list z = [ 0; 1; 2; 3; 4; 5; 6 ]) "push_front/push_back extend list";
  check (focus z = 2) "focus unchanged after push";

  check (front z = 0) "front returns first element";
  check (back z = 6) "back returns last element";

  if !failures = 0 then print_endline "\nAll tests passed."
  else (
    Printf.eprintf "\n%d test(s) failed.\n" !failures;
    exit 1)
