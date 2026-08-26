type nat =
  | Z
  | S of nat

let rec to_int : nat -> int = function
  | Z -> 0
  | S n -> 1 + to_int n

let rec add x y =
  match x with
  | Z -> y
  | S x_prev -> S (add x_prev y)

let rec mult x y =
  match x with
  | Z -> Z
  | S x_prev -> add y (mult x_prev y)

let pred x =
  match x with
  | Z -> Z
  | S x_prev -> x_prev

let rec sub x y =
  match y with
  | Z -> x
  | S y_prev -> sub (pred x) y_prev

let rec less_than x y =
  match x, y with
  | _, Z -> false
  | Z, S _ -> true
  | S x_prev, S y_prev -> less_than x_prev y_prev

let rec div x y =
  match y with
  | Z -> failwith "div: division by zero"
  | S _ ->
    if less_than x y
    then Z
    else S (div (sub x y) y)
