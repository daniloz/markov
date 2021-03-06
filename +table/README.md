# Table Toolbox

**Definition** A *tabular struct* is a matlab struct where every field has the same number of rows

**Definition** A *table* is either

1. a matlab dataset, or
2. a tabular struct

**Definition** A *table field* or simply *field* refers to a field in a table, whether or not 
it takes the form of a dataset or a struct. 

The Table Toolbox provides functionality that is agnostic to choice of 1 or 2. Both forms
of table imply a list of field names `TABLE.FIELDNAMES(ds)` and a collection of records 
analogous to a database table.  

Thus this toolbox does not impose a new table class, prefering home-spun polymorphism in
`TABLE.FIELDNAMES`, `TABLE.SIZE`, `TABLE.SELECT.ROWS` and `TABLE.RMFIELD`.
But the toolbox might be used with user supplied table classes as well, provided these are
modified and other dataset/struct methods are also supported (such as indexing a field, 
subreferencing a field and so forth).  

The Table Toolbox explores a paradigm for organizing multiple calculations involving potentially 
large tables, and exploiting the parallel toolbox in a moderately efficient way. To this end:

**Definition** A *table operator*, or simply *operator* is a function that modifies a table
without altering the number of rows. 

**Definition** A *field immutability respecting operator* or simply *immutable operator* is an operator
that does not alter entries in existing fields, unless it deletes the entire field outright. 

**Definition** A *partition respecting operator* is one that permits simple parallelization on groups
of rows. That is, the computations for one group of rows do not require information about another
groups of rows, nor modify them. 

Given a partition, generated by `TABLE.FEVAL.EQUALPARTITION` or otherwise, any collection of
partition respecting operators can be passed to `TABLE.FEVAL.SEQUENTIAL`, which parallelizes
the computation using parfor. Additionally, any collection of operators that all respect both
field immutability and the partition can be passed to `TABLE.FEVAL.IMMUTABLE`, whether or not the
user is aware of the order in which the operators can or should be applied (since input, new and
removed fields can be inferred by `TABLE.FEVAL.DEPENDENCYDAG`, so can a suitable topological ordering).
This may be used to minimise communication overhead in the parfor call. 
