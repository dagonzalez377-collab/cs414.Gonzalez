type 'a zipper =
  | Empty
  | Zipper of { front : 'a list; focus : 'a; back : 'a list }

let empty : 'a zipper = Empty

let is_empty = function
  | Empty -> true
  | Zipper _ -> false

let rec last_elt = function
  | [] -> invalid_arg "last_elt: empty list"
  | [ x ] -> x
  | _ :: rest -> last_elt rest

let move_left (z : 'a zipper) : 'a zipper =
  match z with
  | Empty -> failwith "move_left: zipper is empty"
  | Zipper { front = []; _ } -> failwith "move_left: focus is already at the front"
  | Zipper { front = x :: rest; focus; back } ->
    Zipper { front = rest; focus = x; back = focus :: back }

let move_right (z : 'a zipper) : 'a zipper =
  match z with
  | Empty -> failwith "move_right: zipper is empty"
  | Zipper { back = []; _ } -> failwith "move_right: focus is already at the back"
  | Zipper { front; focus; back = x :: rest } ->
    Zipper { front = focus :: front; focus = x; back = rest }

(* front/back are stored nearest-to-focus first, so move_left/move_right
   (the operations used on every step through the zipper) are O(1).
   push_front/push_back instead touch the far end of the list, so they're
   O(n); that's the right tradeoff since moving happens far more often
   than pushing. *)
let push_front (x : 'a) (z : 'a zipper) : 'a zipper =
  match z with
  | Empty -> Zipper { front = []; focus = x; back = [] }
  | Zipper { front; focus; back } -> Zipper { front = front @ [ x ]; focus; back }

let push_back (x : 'a) (z : 'a zipper) : 'a zipper =
  match z with
  | Empty -> Zipper { front = []; focus = x; back = [] }
  | Zipper { front; focus; back } -> Zipper { front; focus; back = back @ [ x ] }

let front (z : 'a zipper) : 'a =
  match z with
  | Empty -> failwith "front: zipper is empty"
  | Zipper { front = []; focus; _ } -> focus
  | Zipper { front; _ } -> last_elt front

let back (z : 'a zipper) : 'a =
  match z with
  | Empty -> failwith "back: zipper is empty"
  | Zipper { back = []; focus; _ } -> focus
  | Zipper { back; _ } -> last_elt back

let focus (z : 'a zipper) : 'a =
  match z with
  | Empty -> failwith "focus: zipper is empty"
  | Zipper { focus; _ } -> focus

let of_list (xs : 'a list) : 'a zipper =
  match xs with
  | [] -> Empty
  | x :: rest -> Zipper { front = []; focus = x; back = rest }

let to_list (z : 'a zipper) : 'a list =
  match z with
  | Empty -> []
  | Zipper { front; focus; back } -> List.rev front @ (focus :: back)
