open System.IO

let ReadFile filename =
    File.ReadAllText (filename)

let WriteFile (filename: string, str: string) =
    File.WriteAllText (filename, str)


type Tree =
    | Leaf of int * char        // 
    | Node of int * Tree * Tree
 
let Frequency = function
    | Leaf (f, _) -> f
    | Node (f, _, _) -> f

let FrequencyCompare a b = compare (Frequency a) (Frequency b)
 
let BuildTree charFreqs =
    let leaves = List.map (fun (c,f) -> Leaf (f,c)) charFreqs

    let FrequencySort = List.sortWith FrequencyCompare

    let rec Build = function
        | [] -> failwith "Empty list."
        | [a] -> a
        | a::b::tail ->
            let node = Node(Frequency a + Frequency b, a, b)
            Build (FrequencySort(node::tail))


    let sortedLeaves = FrequencySort leaves
    Build (sortedLeaves)
 
let rec PrintTree = function
  | code, Leaf (f, c) -> printfn "%c\t%d\t%s" c f (String.concat "" (List.rev code));
  | code, Node (_, l, r) ->
      PrintTree ("0"::code, l);
      PrintTree ("1"::code, r)

let rec CreateTable = function
  | code, Leaf (f, c) ->  [(c, String.concat "" (List.rev code))]
  | code, Node (_, l, r) -> List.concat [CreateTable ("0"::code, l); CreateTable ("1"::code, r)]


 
let LetterFrequency str =
  str 
  |> Seq.countBy (fun letter -> letter)
  |> Map.ofSeq
 
let input = ReadFile "input.txt"

let tree = LetterFrequency input |> Map.toList |> BuildTree
printfn "Symbol\tWeight\tHuffman code";
PrintTree ([], tree)

let table = CreateTable ([], tree)


printf "----------------\n\n"

printf "%d\n" table.Length

let rrr = List.fold (fun result (letter : char, code : string) -> result + letter.ToString() + code + "\n") "" table 
printf "%s\n" rrr


let mySmartFunction c = (List.find (fun (ch, code) -> ch = c ) table) |> snd 

let result = List.ofSeq input |> List.map mySmartFunction |> List.fold (+) "" 

printf "%s" result
