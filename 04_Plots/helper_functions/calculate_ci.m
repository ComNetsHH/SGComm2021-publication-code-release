function [edges] = calculate_ci(x,p)
%% Calculations taken from https://de.mathworks.com/matlabcentral/answers/491212-error-bar-with-ci-95-on-bar-graph
% Copyright may be reserved by Adam Danz

edges = std(x(:),'omitnan')/sqrt(sum(~isnan(x(:)))) * tinv(abs([0,1]-(1-p/100)/2),sum(~isnan(x(:)))-1) + mean(x(:),'omitnan'); 

edges = abs(edges-mean(x,'omitnan')); 
end

