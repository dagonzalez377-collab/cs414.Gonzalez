open Zipper

let print_int_list label xs =
  Printf.printf "%s: [%s]\n" label
    (String.concat "; " (List.map string_of_int xs))

let () =
  let z = of_list [ 1; 2; 3; 4; 5 ] in
  print_int_list "initial" (to_list z);
  Printf.printf "focus: %d\n" (focus z);

  let z = move_right z in
  let z = move_right z in
  Printf.printf "focus after two move_right: %d\n" (focus z);

  let z = push_front 0 z in
  let z = push_back 6 z in
  print_int_list "after push_front 0, push_back 6" (to_list z);
  Printf.printf "focus unchanged: %d\n" (focus z);

  Printf.printf "front: %d\n" (front z);
  Printf.printf "back: %d\n" (back z);

  let z = move_left z in
  Printf.printf "focus after move_left: %d\n" (focus z)
