-- Scott Chatham

import Data.Char

-- problem 2
citeAuthor :: String -> String -> String
citeAuthor firstName secondName = secondName ++ ", " ++ firstName

-- problem 3
initials :: String -> String -> String
initials (f:_) (l:_) = [f] ++ "." ++ [l] ++ "."

-- problem 4
title :: (String, String, Int) -> String
title (author, title, year) = title

-- problem 5
citeBook :: (String, String, Int) -> String
citeBook (author, title, year) = title ++ " (" ++ author ++ ", " ++ show year ++ ")"

-- problem 6
bibliography_rec :: [(String, String, Int)] -> String
bibliography_rec [] = ""
bibliography_rec (bk:books) = citeBook bk ++ "\n" ++ bibliography_rec books

-- problem 7
bibliography_fold :: [(String, String, Int)] -> String
bibliography_fold books = let helper x y = x ++ citeBook y ++ "\n" in foldl helper "" books

-- problem 8
averageYear :: [(String, String, Int)] -> Int
averageYear books = sum [year | (author, title, year) <- books] `div` length books

-- problem 9
txt :: String
txt = "[1] and [2] both feature characters who will do whatever it takes to " ++
      "get to their goal, and in the end the thing they want the most ends " ++
      "up destroying them.  in case of [2], this is a whale...1 2 3 4 5"
      
references :: String -> Int
references x = length [num | wrd <- words x, let num = filter (isDigit) wrd, length num > 0]

-- problem 10
citeText :: [(String, String, Int)] -> String -> String
citeText books txt =
  unwords [if length num > 0 then citeBook (books !! ((read num :: Int) - 1)) else wrd | wrd <- words txt,
           let num = filter (isDigit) wrd]

gatsby = ("F. Scott Fitzgerald", "The Great Gatsby", 1925) :: (String, String, Int)
moby = ("Herman Melville", "Moby Dick", 1851) :: (String, String, Int)
