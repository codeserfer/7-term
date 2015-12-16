let StringReversing (s: String) = String(s.ToCharArray() |> Array.rev)

let IsPalindrome s =
    s = (StringReversing s)
    
let rec DecToBin n =
    match n with
    | 0 | 1 -> string n
    | _ ->
        let bit = string (n % 2)
        (DecToBin (n / 2)) + bit


let palindromeList = List.filter (fun x -> IsPalindrome (x.ToString()) && IsPalindrome (DecToBin x)) [1..999999]
let palindromeSum = List.sum palindromeList

printf "7: %d" <| palindromeSum
