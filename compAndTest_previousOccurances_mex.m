mex -v +table\+indexing\mPreviousOccurances.cpp
mex -v +table\+indexing\mPreviousOccurances.cpp -DDEBUG_VERBOSITY=1


I = table.indexing.previousOccurances([15, 1:5, 15, 1:5, 15, 1:5, 15]', [1:3]', 6)

I = table.indexing.previousOccurances([15, 1:5, 15, 1:5, 15, 1:5, 15]', [1:3]', 15, [1:15]')
I = table.indexing.previousOccurances([15, 1:5, 15, 1:5, 15, 1:5, 15]', [1:3]', 15, 1:15)
I = table.indexing.previousOccurances([15, 1:5, 15, 1:5, 15, 1:5, 15]', [1:3]', 15)
I = table.indexing.previousOccurances([15, 1:5, 15, 1:5, 15, 1:5, 15, 15, 15, 15]', [1:3]', 15)
I = table.indexing.previousOccurances([15, 1:5, 15, 1:5, 15, 1:5, 15, 15, 15, 15, 15, 15]', [1:9]', 15)

%%
nKeys = 1000000;
nMaxKey = 100;
keys = randi([1 nMaxKey],nKeys,1);
I = table.indexing.previousOccurances(keys, 1:9);
I = table.indexing.previousOccurances(keys, 1:9, nMaxKey);
