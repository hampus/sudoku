module Main
    where

import Data.List
import Data.Ord
import Data.Char
import Data.Maybe

data Cell = Cell Int Int Int -- row, column, value

-- solve

solve :: [Cell] -> Maybe [Cell]
solve xs = if length xs == 9*9 then Just xs
           else listToMaybe . catMaybes $ [solve (x:xs) | x <- (movesToConsider xs)]

movesToConsider :: [Cell] -> [Cell]
movesToConsider xs = filter moveIsOk (allMovesForCell row col)
    where (row, col) = findFirstEmptyCell xs
          moveIsOk x = not (any (cellsCollide x) xs)

findFirstEmptyCell :: [Cell] -> (Int,Int)
findFirstEmptyCell xs = case (find (not . samePos) (zip allCells (sortedCells xs))) of
        Just (a,b) -> a
        _ -> allCells !! (length xs)
    where allCells = [(x,y) | x <- [0..8], y <- [0..8]] :: [(Int,Int)]
          samePos ((r1,c1), (Cell r2 c2 _)) = r1 == r2 && c1 == c2

allMovesForCell :: Int -> Int -> [Cell]
allMovesForCell row col = [Cell row col x | x <- [1..9]]

cellsCollide :: Cell -> Cell -> Bool
cellsCollide (Cell r1 c1 v1) (Cell r2 c2 v2) =
    (r1 == r2 && c1 == c2) || (v1 == v2 && (r1 == r2 || c1 == c2 || (zone r1 c1) == (zone r2 c2)))

zone :: Int -> Int -> Int
zone row col = 3*y + x
    where x = quot col 3
          y = quot row 3

-- parse

parseS :: String -> [Cell]
parseS input = [Cell (indexToRow i) (indexToCol i) (digitToInt c) | (i,c) <- zip [0..81] chars, c /= '.']
    where chars = concat (lines input)

indexToRow index = quot index 9
indexToCol index = rem index 9

-- display

sortedCells :: [Cell] -> [Cell]
sortedCells = sortBy (comparing cellIndex)

formatS :: Maybe [Cell] -> String
formatS = (wrap 18) . concat . (map formatCell) . sortedCells . (fromMaybe [])

formatCell :: Cell -> String
formatCell (Cell _ _ value) = if value == 0 then ". "
                              else (show value) ++ " "

wrap :: Int -> String -> String
wrap width [] = []
wrap width xs = line ++ "\n" ++ (wrap width rest)
    where (line, rest) = splitAt width xs

cellIndex :: Cell -> Int
cellIndex (Cell row col _) = row*9 + col


-- main

main = interact (formatS . solve . parseS)

