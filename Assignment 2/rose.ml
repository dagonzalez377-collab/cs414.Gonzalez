type 'a rose = Node of 'a * 'a rose list

let rec size t =
  match t with
  | Node (_, children) -> 1 + List.fold_left (fun acc c -> acc + size c) 0 children

let rec map f t =
  match t with
  | Node (x, children) -> Node (f x, List.map (map f) children)

let rec fold f t =
  match t with
  | Node (x, children) -> f x (List.map (fold f) children)
