# Project-K23a
![Build status](https://github.com/joannakats/Project-K23a/workflows/build/badge.svg)

## Team Members
Katsouli Ioanna (1115201400067)<br/>
Konomi Marina (1115201700054)<br/>
Leonidas-Panagiotis Papadakos (1115201700117)<br/>

## Data Structures
*A picture is worth a thousand words.*
![Structures diagram](./structs.svg)

Hashtable is used, as it helps to form smaller lists and makes the search procedure faster.<br>Faster, because it has average time complexity O(1) for search, insertion and deletion<br/> (and worst time complexity O(n)).<br/>
Single-linked lists has O(1) for insertion and deletion and O(n) for search procedure.

## Build
Run `make`, to build the executables and unit tests<br/>
To run the testing executables, (found in tests/), run `make check`.

## Usage
`./specs [-e buckets (default: 10007)] -x dataset_x -w dataset_w [-o FILE (default: stdout)]`

## Notes/Assumptions
* The unit testing library used is [acutest](https://github.com/mity/acutest).
* It is assumed that within the directory of `Dataset X`, there will be only site folders (e.g. www.ebay.com) and .json files.
* The way we print the cliques as distinct pairs of specs is outlined in `print_pairs()`, within [hashtable.c](./src/hashtable.c). There was a discussion on this in [this Piazza question](https://piazza.com/class/kfduqabor94zw?cid=11) and verbally, in the weekly Zoom meetings.
* The program outputs to stdout by default, which is also suggested in the aforementioned Piazza discussion. Use `-o somefile.csv` or redirection, to point the data elsewhere.
