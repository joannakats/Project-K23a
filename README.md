# Project-K23a
![Build status](https://github.com/joannakats/Project-K23a/workflows/build/badge.svg)

## Team Members
Katsouli Ioanna (1115201400067)<br/>
Konomi Marina (1115201700054)<br/>
Leonidas-Panagiotis Papadakos (1115201700117)<br/>


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
* Stopword list: [english.txt](https://github.com/Alir3z4/stop-words/blob/master/english.txt) from [Alir3z4/stop-words](https://github.com/Alir3z4/stop-words).
* Using the log10() function, because the numeric results in the exercise explanation point to it instead of log9) or log2().
* It is assumed that the sets (training, testing e.t.c.) are at least as big at the BATCH_SIZE.

## Validation
* At the beginnig of every round, validation() creates new structures of cliques for the sake of the algorithm and destroys them at the end of every round.
* These cliques are created according to the predictions of the model.
* The idea is: we insert relations of specs, if and only if, these relations don't cause any conflicts in these structures. In case they create conflicts we keep a record of: the specs that caused the conflicts, the type of the relation and the prediction of the model (aka possibility). These records are kept in a form of a list.
* For every round, after we've inserted everything we wanted into structures we resolve the conflicts by traversing the list of conflicts. According to the type of the conflict we call its corresponding function.
* The decision whether we will keep the relation that caused the conflict or not is made according to the possibilities and the number of relations, which should be less or equal to 5 (this number is chosen empirically considering the delay of the progam and the reduction of conflicts). 
* Every time we change a relation we update the weights.
* validation() continues resolving conflicts until the number of conflicts drops by 45%.
