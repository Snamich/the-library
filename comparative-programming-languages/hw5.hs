-- Scott Chatham

-- Necessary imports
import Control.Applicative ((<$>),liftA,liftA2)
import Data.Map
import Text.Parsec hiding ((<$>))
import Text.Parsec.Expr
import Text.Parsec.Language (emptyDef)
import Text.Parsec.String (Parser)
import qualified Text.Parsec.Token as P


--------- AST Nodes ---------

-- Variables are identified by their name as string
type Variable = String

-- Values are either integers or booleans
data Value = IntVal Int       -- Integer value
          | BoolVal Bool     -- Boolean value

-- Expressions are variables, literal values, unary and binary operations
data Expression = Var Variable                    -- e.g. x
                | Val Value                       -- e.g. 2
                | BinOp Op Expression Expression  -- e.g. x + 3
                | Assignment Variable Expression  -- e.g. x = 3

-- Statements are expressions, conditionals, while loops and sequences
data Statement = Expr Expression                   -- e.g. x = 23
              | If Expression Statement Statement -- if (e) {s1} else {s2}
              | While Expression Statement        -- while (e) {s}
              | Sequence Statement Statement      -- s1; s2
              | Skip                              -- no-op
              | For Variable Expression Expression Statement -- for x in 1 to 10 do s1 end

-- All binary operations
data Op = Plus         --  +  :: Int -> Int -> Int
        | Minus        --  -  :: Int -> Int -> Int
        | Times        --  *  :: Int -> Int -> Int
        | GreaterThan  --  >  :: Int -> Int -> Bool
        | Equals       --  == :: Int -> Int -> Bool
        | LessThan     --  <  :: Int -> Int -> Bool
        | LessThanEquals

-- The `Store` is an associative map from `Variable` to `Value` representing the memory
type Store = Map Variable Value

--------- Parser ---------

-- The Lexer
lexer = P.makeTokenParser (emptyDef{
  P.identStart = letter,
  P.identLetter = alphaNum,
  P.reservedOpNames = ["+", "-", "*", "!", ">", "=", "==", "<", "<="],
  P.reservedNames = ["true", "false", "if", "in", "then", "else", "while", "end", "to", "do", "for"]
})

-- The Parser

-- Number literals
numberParser :: Parser Value
numberParser = (IntVal . fromIntegral) <$> P.natural lexer

-- Boolean literals
boolParser :: Parser Value
boolParser =  (P.reserved lexer "true" >> return (BoolVal True))
          <|> (P.reserved lexer "false" >> return (BoolVal False))

-- Literals and Variables
valueParser :: Parser Expression
valueParser =  Val <$> (numberParser <|> boolParser)
          <|> Var <$> P.identifier lexer

-- -- Expressions
exprParser :: Parser Expression
exprParser = liftA2 Assignment
                    (try (P.identifier lexer >>= (\v ->
                          P.reservedOp lexer "=" >> return v)))
                    exprParser
          <|> buildExpressionParser table valueParser
    where table = [[Infix (op "*" (BinOp Times)) AssocLeft]
                  ,[Infix (op "+" (BinOp Plus)) AssocLeft]
                  ,[Infix (op "-" (BinOp Minus)) AssocLeft]
                  ,[Infix (op ">" (BinOp GreaterThan)) AssocLeft]
                  ,[Infix (op "==" (BinOp Equals)) AssocLeft]
                  ,[Infix (op "<" (BinOp LessThan)) AssocLeft]
                  ,[Infix (op "<=" (BinOp LessThanEquals)) AssocLeft]]
          op name node = (P.reservedOp lexer name) >> return node

-- Sequence of statements
stmtParser :: Parser Statement
stmtParser = stmtParser1 `chainl1` (P.semi lexer >> return Sequence)

-- Single statements
stmtParser1 :: Parser Statement
stmtParser1 = (Expr <$> exprParser)
          <|> do
              P.reserved lexer "if"
              cond <- exprParser
              P.reserved lexer "then"
              the <- stmtParser
              P.reserved lexer "else"
              els <- stmtParser
              P.reserved lexer "end"
              return (If cond the els)
          <|> do
              P.reserved lexer "while"
              cond <- exprParser
              P.reserved lexer "do"
              body <- stmtParser
              P.reserved lexer "end"
              return (While cond body)
          <|> do
              P.reserved lexer "for"
              var <- P.identifier lexer
              P.reserved lexer "in"
              low <- exprParser
              P.reserved lexer "to"
              up <- exprParser
              P.reserved lexer "do"
              body <- stmtParser
              P.reserved lexer "end"
              return (For var low up body)

-------- Helper functions --------

-- Lift primitive operations on IntVal and BoolVal values
liftIII :: (Int -> Int -> Int) -> Value -> Value -> Value
liftIII f (IntVal x) (IntVal y) = IntVal $ f x y
liftIIB :: (Int -> Int -> Bool) -> Value -> Value -> Value
liftIIB f (IntVal x) (IntVal y) = BoolVal $ f x y

-- Apply the correct primitive operator for the given Op value
applyOp :: Op -> Value -> Value -> Value
applyOp Plus        = liftIII (+)
applyOp Minus       = liftIII (-)
applyOp Times       = liftIII (*)
applyOp GreaterThan = liftIIB (>)
applyOp Equals      = liftIIB (==)
applyOp LessThan    = liftIIB (<)
applyOp LessThanEquals = liftIIB (<=)

-- parse and print (pp) the given while programs
pp :: String -> IO ()
pp input = case (parse stmtParser "" input) of
    Left err -> print err
    Right x  -> print x

-- parse and run the given while programs
run :: (Show v) => (Parser n) -> String -> (n -> Store -> v) -> IO ()
run parser input eval = case (parse parser "" input) of
    Left err -> print err
    Right x  -> print (eval x empty)

-- parse and run the given while programs using monads
runMonad :: String -> Maybe Store
runMonad input = proc (parse stmtParser "" input)
    where proc (Right x) = snd `fmap` runImperative (evalS_monad x) empty
          proc _         = Nothing

-- getter and setter for monads
getVar :: Variable -> Imperative Value
getVar var = Imperative (\store -> ((Data.Map.lookup var store) >>= (\v -> Just (v, store))))

setVar :: Variable -> Value -> Imperative Value
setVar var val = Imperative (\store -> Just (val, Data.Map.insert var val store))

-- imperative monad
newtype Imperative a = Imperative {
    runImperative :: Store -> Maybe (a, Store)
}

instance Monad Imperative where
    return a = Imperative (\s -> Just (a,s))
    b >>= f = Imperative (\s -> do (v1,s1) <- (runImperative b) s
                                   runImperative (f v1) s1)

-- problem 1
instance Show Value where
  show (IntVal x) = show x
  show (BoolVal b) = show b

instance Show Op where
  show Plus = show "+"
  show Minus = show "-"
  show Times = show "*"
  show GreaterThan = show ">"
  show Equals = show "=="
  show LessThan = show "<"
  show LessThanEquals = show "<="

instance Show Expression where
  show (Var v) = show v
  show (Val v) = show v
  show (BinOp op e1 e2) = show e1 ++ show op ++ show e2
  show (Assignment v e) = show v ++ "=" ++ show e
    
instance Show Statement where
  show (Expr e) = show e
  show (If e s1 s2) = "if" ++ show e ++ show s1 ++ "else" ++ show s2
  show (While e s) = "while" ++ show e ++ show s
  show (Sequence s1 s2) = show s1 ++ ";" ++ show s2
  show Skip = "no-op"
  show (For v e1 e2 s) = "for" ++ show v ++ "in" ++ show e1 ++ "to" ++ show e2 ++ "do" ++ show s ++ "end"

-- problem 2
evalE :: Expression -> Store -> (Value, Store)
evalE (BinOp op e1 e2) s = (applyOp op e1' e2', s'')
    where (e1', s')  = evalE e1 s
          (e2', s'') = evalE e2 s'
evalE (Var x) s =
  case Data.Map.lookup x s of
    Nothing -> error "variable used without being defined"
    Just v -> (v, s)
evalE (Val v) s = (v, s)
evalE (Assignment x e) s = (e', Data.Map.insert x e' s')
    where (e', s') = evalE e s

-- problem 3
evalS :: Statement -> Store -> Store
evalS w@(While e s1) s = case (evalE e s) of
                          (BoolVal True, s')  -> let s'' = evalS s1 s' in evalS w s''
                          (BoolVal False, s') -> s'
                          _                   -> error "condition must be a BoolVal"
evalS Skip s = s
evalS (Expr e) s = let (v, s') = evalE e s in s'
evalS (Sequence s1 s2) s = let s' = evalS s1 s in evalS s2 s'
evalS (If e s1 s2) s = case (evalE e s) of
    (BoolVal True, s')  -> let s'' = evalS s1 s' in s''
    (BoolVal False, s') -> let s''' = evalS s2 s' in s'''
    _                   -> error "condition must be a BoolVal"
evalS (For x e1 e2 s1) s = evalS (While (BinOp LessThanEquals (Var x) e2) (Sequence s1 s2)) s'
   where (v, s') = evalE (Assignment x e1) s
         s2 = (Expr (Assignment x (BinOp Plus (Var x) (Val (IntVal 1)))))

-- problem 4
evalE_maybe :: Expression -> Store -> Maybe (Value, Store)
evalE_maybe (BinOp o e1 e2) s  = do (e1', s') <- evalE_maybe e1 s
                                    (e2', s'') <- evalE_maybe e2 s'
                                    return (applyOp o e1' e2', s'')
evalE_maybe (Var x) s          = do x' <- Data.Map.lookup x s
                                    return (x', s)
evalE_maybe (Val v) s          = Just (v, s)
evalE_maybe (Assignment x e) s = do (e', s') <- evalE_maybe e s
                                    s'' <- Just $ Data.Map.insert x e' s'
                                    return (e', s'')

evalS_maybe :: Statement -> Store -> Maybe Store
evalS_maybe w@(While e s1) s   = do (e', s') <- evalE_maybe e s
                                    case e' of
                                      BoolVal True -> do s'' <- evalS_maybe s1 s'
                                                         evalS_maybe w s''
                                      BoolVal False -> return s'
                                      _ -> Nothing
evalS_maybe Skip s             = Just s
evalS_maybe (Sequence s1 s2) s = do s' <- evalS_maybe s1 s
                                    s'' <- evalS_maybe s2 s'
                                    return s''
evalS_maybe (Expr e) s         = do (e', s') <- evalE_maybe e s
                                    return s'
evalS_maybe (If e s1 s2) s     = do (e', s') <- evalE_maybe e s
                                    case e' of
                                      BoolVal True -> evalS_maybe s1 s'
                                      BoolVal False -> evalS_maybe s2 s'
                                      _ -> Nothing
evalS_maybe (For x e1 e2 s1) s = do (v, s') <- evalE_maybe (Assignment x e1) s
                                    evalS_maybe (While (BinOp LessThanEquals (Var x) e2) (Sequence s1 s2)) s'
  where s2 = (Expr (Assignment x (BinOp Plus (Var x) (Val (IntVal 1)))))

-- problem 5
evalE_monad :: Expression -> Imperative Value
evalE_monad (BinOp o a b)    = do a' <- evalE_monad a
                                  b' <- evalE_monad b
                                  return $ applyOp o a' b'
evalE_monad (Var x)          = getVar x
evalE_monad (Val v)          = return v
evalE_monad (Assignment x e) = do e' <- evalE_monad e
                                  setVar x e'

evalS_monad :: Statement -> Imperative ()
evalS_monad w@(While e s1)   = do e' <- evalE_monad e
                                  case e' of
                                    BoolVal True -> do evalS_monad s1
                                                       evalS_monad w
                                    _ -> return ()
evalS_monad Skip             = return ()
evalS_monad (Sequence s1 s2) = do evalS_monad s1
                                  evalS_monad s2
evalS_monad (Expr e)         = do e' <- evalE_monad e
                                  return ()
evalS_monad (If e s1 s2)     = do e' <- evalE_monad e
                                  case e' of
                                    BoolVal True -> do evalS_monad s1
                                    BoolVal False -> do evalS_monad s2
                                    _ -> Imperative (\s -> Nothing)
evalS_monad (For x e1 e2 s1) = do e' <- evalE_monad (Assignment x e1)
                                  evalS_monad (While (BinOp LessThanEquals (Var x) e2) (Sequence s1 s2))
  where s2 = (Expr (Assignment x (BinOp Plus (Var x) (Val (IntVal 1)))))
                                  

-- problem 6
-- for statements are included in the previous problems
