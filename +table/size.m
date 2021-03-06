function [rows,cols] = size(ds,k)
% function [rows,cols] = size(ds,k)

if isa(ds,'dataset'),
    [rows,cols] = size(ds);
elseif isa(ds,'struct'),
    vn = fieldnames(ds);
    if isempty(vn),
        rows = 0;
        cols = 0;
    else
        rows = size(ds.(vn{1}),1);
        cols = length(vn);
    end
else
    error('Expecting struct or dataset');
end

if nargin>1,
   switch k
       case 1 
           % rows = rows
       case 2
           rows = cols;
       otherwise
           error('Expecting dsSize(ds,1) or dsSize(ds,2)');
   end
end

if nargout<2 && nargin==1
   rows = [rows, cols]; 
end

end