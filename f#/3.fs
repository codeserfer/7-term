let b = 2.0
let k = 6

let rec calc ai num k acc =
    if k = 0 then acc
    else
      let next = ai - 1.0 - 1.0 / (sqrt num)
      if ai < 0.0 then calc next (num + 1.0) (k-1) (ai::acc)
      else calc next (num + 1.0) k acc
 
printf "%A" <| calc b 1.0 k []
