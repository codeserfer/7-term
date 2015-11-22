let anInt = 5

let rec CalculateSequence n = 
    if n <= 2 then n
    else CalculateSequence (n - 1)*CalculateSequence (n-2)
System.Console.WriteLine(CalculateSequence anInt)
