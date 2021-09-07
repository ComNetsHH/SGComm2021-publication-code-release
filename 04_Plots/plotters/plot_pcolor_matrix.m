function [H,ax] = plot_pcolor_matrix(experiments,seqAxis,rtxAxis)

data = Inf(max(seqAxis),size(rtxAxis,2));

for experimentID = 1:size(experiments,2)
    if(isfield(experiments(experimentID).results,'avgAoI'))
        % find rtxAxis position
        [rtxValue, i] = find(rtxAxis==experiments(experimentID).results.rtxTimer,1);
        data(experiments(experimentID).results.numSeq,i) = experiments(experimentID).results.avgAoI;
    end
end

data = data./1000;
data(data>14.5) = 14.5;
H = figure('Units','points','Position',[0 0 12*21 8*21],'PaperPositionMode','auto');
colormap(flipud(gray));
box on;
h =colorbar;

h.Label.String = "AoI [s]";
ax = pcolor(seqAxis,rtxAxis,min(Inf,data'));

end

