let data = "The +- quick 345 brown fox jumps over the lazy dog"

let result = 
    data.ToLower().ToCharArray() 
    |> Seq.filter (fun c -> Char.IsLetter(c)) 
    |> Seq.groupBy (fun c ->c) 
    |> Seq.map (fun (c, s) -> (c, Seq.length s)) 
    |> Seq.maxBy snd 

printfn "%A" result
