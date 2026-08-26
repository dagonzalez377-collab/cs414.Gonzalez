type binary_tree =
  | Empty
  | Node of int * binary_tree * binary_tree

let rec height tree =
  match tree with
  | Empty -> 0
  | Node (_, left, right) ->
      1 + max (height left) (height right)

let rec prune tree =
  match tree with
  | Empty -> Empty
  | Node (_, Empty, Empty) -> Empty
  | Node (value, left, right) ->
      Node (value, prune left, prune right)

let level_traversal tree =
  let rec values_of nodes =
    match nodes with
    | [] -> []
    | Empty :: rest -> values_of rest
    | Node (value, _, _) :: rest -> value :: values_of rest
  in
  let rec children_of nodes =
    match nodes with
    | [] -> []
    | Empty :: rest -> children_of rest
    | Node (_, left, right) :: rest -> left :: right :: children_of rest
  in
  let rec traverse nodes =
    match nodes with
    | [] -> []
    | _ -> values_of nodes @ traverse (children_of nodes)
  in
  traverse [tree]
