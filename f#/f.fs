let anInt = 5

let rec CalculateSequence n = 
    if n <= 2 then n
    else CalculateSequence (n - 1)*CalculateSequence (n-2)
System.Console.WriteLine(CalculateSequence anInt)
let data = "The +- quick 345 brown fox jumps over the lazy dog"

let result = 
    data.ToLower().ToCharArray() 
    |> Seq.filter (fun c -> Char.IsLetter(c)) 
    |> Seq.groupBy (fun c ->c) 
    |> Seq.map (fun (c, s) -> (c, Seq.length s)) 
    |> Seq.maxBy snd 

printfn "%A" result
let b = 2.0
let k = 6

let rec calc ai num k acc =
    if k = 0 then acc
    else
      let next = ai - 1.0 - 1.0 / (sqrt num)
      if ai < 0.0 then calc next (num + 1.0) (k-1) (ai::acc)
      else calc next (num + 1.0) k acc
 
printf "%A" <| calc b 1.0 k []
let isNeededNumer n = 
    if (n % 3 = 0) || (n % 5 = 0) then n
    else 0

let sum = List.sumBy (fun elem -> isNeededNumer (elem)) [1 .. 1000]

printf "%d" sum
let rec Gcd a b =
    if b = 0L then a
    else Gcd b (a % b)

let Lcm a b = 
    abs (a*b)/(Gcd a b)


let result =  List.fold Lcm 1L [1L .. 20L]
printf "%d" result
let Discriminant a b c =
    b*b - 4*a*c


let Solve a b c =
    let discriminant = Discriminant a b c 
    if discriminant > 0 then 2
    else if discriminant = 0 then 1
    else 0

printf "%d"  <| Solve 1 4 4
