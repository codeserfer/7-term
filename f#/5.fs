let rec Gcd a b =
    if b = 0L then a
    else Gcd b (a % b)

let Lcm a b = 
    abs (a*b)/(Gcd a b)


let result =  List.fold Lcm 1L [1L .. 20L]
printf "%d" result
