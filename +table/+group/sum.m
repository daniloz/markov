function avgX = sum(X,varargin)
f = @(x) nansum(x);
avgX = table.group.fun(X,f,varargin{:});
end