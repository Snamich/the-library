-- Scott Chatham

import Data.List

-- problem 1
val :: BST k v -> Maybe v
val Empty = Nothing
val (Node _ v _ _) = Just v

-- problem 2
size :: BST k v -> Int
size Empty = 0
size (Node k v left right) = 1 + size left + size right

-- problem 3
ins :: (Ord k) => k -> v -> BST k v -> BST k v
ins k v Empty = Node k v Empty Empty
ins k1 v1 (Node k2 v2 left right)
  | k1 == k2 = Node k1 v1 left right
  | k1 < k2 = Node k2 v2 (ins k1 v1 left) right
  | k1 > k2 = Node k2 v2 left (ins k1 v1 right)

-- problem 4
instance (Show v) => Show (BST k v) where
  show Empty = ""
  show (Node k v left right) = "(" ++ show left ++ show v ++ show right ++ ")"

-- problem 5
instance Show JSON where
  show (JStr s) = show s
  show (JNum n) = show n
  show (JArr js) = "[" ++ intercalate "," (map show js) ++ "]"
  show (JObj os) = "{" ++ intercalate "," (map (\(k, v) -> show k ++ ":" ++ show v) os) ++ "}"

-- problem 6
class Json a where
  toJson :: a -> JSON
  fromJson :: JSON -> a

instance Json Double where
  toJson x = JNum x
  fromJson (JNum n) = n
                           
instance (Json a) => Json [a] where
  toJson xs = JArr (map toJson xs)
  fromJson (JArr xs) = map fromJson xs

-- data
data BST k v = Empty |
               Node k v (BST k v) (BST k v)

data JSON = JStr String
          | JNum Double
          | JArr [JSON]
          | JObj [(String, JSON)]
