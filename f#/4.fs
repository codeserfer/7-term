let isNeededNumer n = 
    if (n % 3 = 0) || (n % 5 = 0) then n
    else 0

let sum = List.sumBy (fun elem -> isNeededNumer (elem)) [1 .. 1000]

printf "%d" sum
