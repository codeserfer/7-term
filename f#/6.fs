let Discriminant a b c =
    b*b - 4*a*c


let Solve a b c =
    let discriminant = Discriminant a b c 
    if discriminant > 0 then 2
    else if discriminant = 0 then 1
    else 0

printf "%d"  <| Solve 1 4 4
