-- Scott Chatham

-- problem 1
myFoldl :: (a -> b -> a) -> a -> [b] -> a
myFoldl _ z [] = z
myFoldl f z (x:xs) = myFoldl f (f z x) xs

-- problem 2
myReverse :: [a] -> [a]
myReverse xs = foldl (\acc x -> [x] ++ acc) [] xs

-- problem 3
myFoldr :: (a -> b -> b) -> b -> [a] -> b
myFoldr f acc xs = foldl (flip f) acc xs

-- problem 4
myFoldl2 :: (a -> b -> a) -> a -> [b] -> a
myFoldl2 f acc xs = foldr (flip f) acc xs

-- problem 5
isUpper :: Char -> Bool
isUpper c = c `elem` ['A'..'Z']

-- problem 6
onlyCapitals1 :: String -> String
onlyCapitals1 str = filter isUpper str

-- problem 7
onlyCapitals2 :: String -> String
onlyCapitals2 str = [c | c <- str, isUpper c]

-- problem 8
onlyCapitals3 :: String -> String
onlyCapitals3 [] = []
onlyCapitals3 (x:xs) | isUpper x = x : onlyCapitals3 xs
                     | otherwise = onlyCapitals3 xs

-- problem 9
divRemainder :: Int -> Int -> (Int, Int)
divRemainder a b = (a `div` b, a `mod` b)

-- problem 10
digitSum :: Int -> Int
digitSum 0 = 0
digitSum num = num `mod` 10 + (digitSum $ num `div` 10)

-- problem 11
sayNum :: String -> String
sayNum num = sayNumHelper (reverse num) 0

-- sayNumHelper breaks a number into as many groups of three as it can
-- it's ugly, but I spaced out the linebreaks the way I did on the first pattern so you can see each place easier
sayNumHelper :: String -> Int -> String
sayNumHelper [] _ = ""
sayNumHelper (z:y:x:xs) n
  | myDigitToInt y == 1 = sayNumHelper xs (n + 1) ++
                          (if (myDigitToInt x) == 0 then "" else ones !! (myDigitToInt x) ++ "hundred ") ++
                          teens !! (myDigitToInt z) ++
                          thousandths !! n
  | otherwise = sayNumHelper xs (n + 1) ++
                (if (myDigitToInt x) == 0 then "" else ones !! (myDigitToInt x) ++ "hundred ") ++
                tens !! (myDigitToInt y) ++
                ones !! (myDigitToInt z) ++
                thousandths !! n
sayNumHelper (z:y:_) n
  | myDigitToInt y == 1 = teens !! (myDigitToInt z) ++ thousandths !! n
  | otherwise = tens !! (myDigitToInt y) ++ ones !! (myDigitToInt z) ++ thousandths !! n
sayNumHelper (z:_) n = ones !! (myDigitToInt z) ++ thousandths !! n

-- #11 data
ones = ["", "one ", "two ", "three ", "four ", "five ", "six ", "seven ", "eight ", "nine "]
tens = ["", "", "twenty ", "thirty ", "fourty ", "fifty ", "sixty ", "seventy ", "eighty ", "ninety "]
teens = ["ten ", "eleven ", "twelve ", "thirteen ", "fourteen ", "fifteen ", "sixteen ", "seventeen ",
         "eighteen ", "nineteen "]
thousandths = ["", "thousand ", "million ", "billion ", "trillion ", "quadrillion ", "quintillion ",
               "sextillion ", "septillion ", "octillion ", "nonillion ", "decillion ", "undecillion ", 
               "duodecillion ", "tredecillion ", "quattuordecillion ", "quindecillion ", "sexdecillion ",
               "septendecillion ", "octodecillion ", "novemdecillion ", "vigintillion "]

-- auxiliary functions
-- can't import Data.Char because of isUpper clash so I had to write my own digitToInt
myDigitToInt :: Char -> Int
myDigitToInt num = read [num] :: Int
