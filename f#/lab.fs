open System.IO

let ReadFile filename =
    File.ReadAllText (filename)

let WriteFile (filename: string, str: string) =
    File.WriteAllText (filename, str)

type Tree =
    | Leaf of int * char
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
  | code, Leaf (f, c) -> printfn "%c\t%d\t%s" c f (String.concat "" (List.rev code))
  | code, Node (_, l, r) ->
      PrintTree ("0"::code, l)
      PrintTree ("1"::code, r)

let rec GetCodes = function
  | code, Leaf (f, c), table -> List.append table [String.concat "" (List.rev code)]
  | code, Node (_, l, r), table ->
      List.append (List.append table (GetCodes ("0"::code, l, table))) (GetCodes ("1"::code, r, table))

let rec GetLetters = function
  | code, Leaf (f, c), table -> List.append table [c]
  | code, Node (_, l, r), table ->
      List.append (List.append table (GetLetters ("0"::code, l, table))) (GetLetters ("1"::code, r, table))

let rec CreateTable = function
  | code, Leaf (f, c) ->  [(c, String.concat "" (List.rev code))]
  | code, Node (_, l, r) -> List.concat [CreateTable ("0"::code, l); CreateTable ("1"::code, r)]
 
let LetterFrequency str =
  str 
  |> Seq.countBy (fun letter -> letter)
  |> Map.ofSeq

let rec ToOctets (str : string) =
    if str.Length % 8 = 0 then str 
    else ToOctets (str + "0")

let rec BinaryToDecimal (binary : string) decimal =
    if binary.Length = 0 then decimal
    else
        match binary.Chars 0 with
            | '0' -> BinaryToDecimal (binary.Substring 1) (decimal * 2)
            | '1' -> BinaryToDecimal (binary.Substring 1) (decimal * 2 + 1)
            | _   -> 0   

let rec EncodedToBytes (str : string) (numberOfOctets : int) octetCounter =
    if octetCounter = numberOfOctets then []
    else List.append [byte (BinaryToDecimal str.[0..7] 0)] (EncodedToBytes str.[8..str.Length - 1] numberOfOctets (octetCounter + 1))

[<EntryPoint>]
let main argv = 
//    if argv.Length <> 3
//    then
//        printf "%s" "Error!"
//        -1
//    else
//        let inputFile = argv.GetValue(0)
//        let outputFile = argv.GetValue(1)
//        let mode = argv.GetValue(2)
//        0
    
    //let input = ReadFile "input.txt"

    let input = "aaabbgvvg"

    let tree = LetterFrequency input |> Map.toList |> BuildTree
    //printfn "Symbol\tWeight\tHuffman code";
    //PrintTree ([], tree)

    let table = CreateTable ([], tree)

    let length = table.Length
    
    let stringTable = List.fold (fun result (letter : char, code : string) ->  result + letter.ToString() + code + "\n") "" table
    
    let Encode letterToEncode =
        (List.find (fun (letter, code) -> letter = letterToEncode) table) |> snd

    let result = List.ofSeq input |> List.map Encode |> List.fold (+) ""
    let resultToOctets = ToOctets result

    let bytes = EncodedToBytes resultToOctets (resultToOctets.Length /  8) 0

    let output = table.Length.ToString() + "\n" +
        stringTable +
        input.Length.ToString() + "\n" +
        (System.Text.Encoding.ASCII.GetString (List.toArray bytes)) 

    printf "%s" output

    0
