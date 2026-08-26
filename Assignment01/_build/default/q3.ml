type 'a gtree =
  | Empty
  | Node of 'a list * 'a gtree list

let rec height t =
  match t with
  | Empty -> 0
  | Node (_, children) ->
    1 + List.fold_left (fun acc c -> max acc (height c)) 0 children

let rec preorder f t =
  match t with
  | Empty -> ()
  | Node (keys, children) ->
    List.iter f keys;
    List.iter (preorder f) children

let rec postorder f t =
  match t with
  | Empty -> ()
  | Node (keys, children) ->
    List.iter (postorder f) children;
    List.iter f keys
