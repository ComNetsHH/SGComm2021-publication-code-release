%% Static Config
close all
colororder(publicationColors)
t = tiledlayout(2,2, 'TileSpacing', 'compact');
fig = gcf;
nexttile
hold on;
fig.Units = 'points';
fig.Position =[0 0 12*21 9.175*21];
fig.PaperPositionMode = 'auto';

experiments =  load_experiments(path_to_processed_results,bar_graph_experiment_names(1));
maxSeq = 0;
maxRng = 0;
numBBNodes = 0;
for experimentID = 1:size(experiments,2)
    
    if(isfield(experiments(experimentID).results,'rngID'))
        experiments(experimentID).results.rngID = str2double(experiments(experimentID).results.rngID);
    else
        experiments(experimentID).results.rngID = 1;
    end
    
    if(experiments(experimentID).results.numSeq>maxSeq)
        maxSeq=experiments(experimentID).results.numSeq;
    end
    if(experiments(experimentID).results.rngID>maxRng)
        maxRng = experiments(experimentID).results.rngID;
    end
    if(isfield(experiments(1).results,'rtxMean'))
        if(size(experiments(1).results.rtxMean,2)>numBBNodes)
            numBBNodes = size(experiments(1).results.rtxMean,2);
        end
    else
        numBBNodes=1;
    end
end

xData = repmat([1:maxSeq],maxRng,1);
yData = Inf(maxRng,maxSeq);

for experimentID = 1:size(experiments,2)
    if(isfield(experiments(experimentID).results,'avgAoI'))
        if(yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
            > experiments(experimentID).results.avgAoI)
            yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
        end
    end
end
% Squash Inf rows if missing data
yData(all(isinf(yData),2),:)=[];
% Only select first numRuns entries
if(size(yData,1)>numRuns)
    yData = yData(1:numRuns,:);
    xData = xData(1:numRuns,:);
end
global confidenceLevel;
yData(isinf(yData)) = NaN;
yDataCI = zeros(size(xData,2),2);
for xID = 1:size(xData,2)
   yDataCI(xID,:) =  calculate_ci(yData(:,xID),confidenceLevel);
end
yDataMean = mean(yData,1,'omitnan');

y(1,1) = yDataMean(1);
y(3,1) = yDataMean(end);
[y(2,1) bestk] = min(yDataMean);
disp(strcat("### INFO: Best optimal number of streams for: ", bar_graph_experiment_names(1), " is at k = ", num2str(bestk)));


errorplus(1,1) = yDataCI(1,1);
errorplus(3,1) = yDataCI(end,1);
errorplus(2,1) = yDataCI(bestk,1);

experiments =  load_experiments(path_to_processed_results,bar_graph_experiment_names(2));
maxSeq = 0;
maxRng = 0;
numBBNodes = 0;
for experimentID = 1:size(experiments,2)
    
    if(isfield(experiments(experimentID).results,'rngID'))
        experiments(experimentID).results.rngID = str2double(experiments(experimentID).results.rngID);
    else
        experiments(experimentID).results.rngID = 1;
    end
    
    if(experiments(experimentID).results.numSeq>maxSeq)
        maxSeq=experiments(experimentID).results.numSeq;
    end
    if(experiments(experimentID).results.rngID>maxRng)
        maxRng = experiments(experimentID).results.rngID;
    end
    if(isfield(experiments(1).results,'rtxMean'))
        if(size(experiments(1).results.rtxMean,2)>numBBNodes)
            numBBNodes = size(experiments(1).results.rtxMean,2);
        end
    else
        numBBNodes=1;
    end
end

xData = repmat([1:maxSeq],maxRng,1);
yData = Inf(maxRng,maxSeq);

for experimentID = 1:size(experiments,2)
    if(isfield(experiments(experimentID).results,'avgAoI'))
        if(yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
            > experiments(experimentID).results.avgAoI)
            yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
        end
    end
end
% Squash Inf rows if missing data
yData(all(isinf(yData),2),:)=[];
% Only select first numRuns entries
if(size(yData,1)>numRuns)
    yData = yData(1:numRuns,:);
    xData = xData(1:numRuns,:);
end
global confidenceLevel;
yData(isinf(yData)) = NaN;
yDataCI = zeros(size(xData,2),2);
for xID = 1:size(xData,2)
   yDataCI(xID,:) =  calculate_ci(yData(:,xID),confidenceLevel);
end
yDataMean = mean(yData,1,'omitnan');

y(1,2) = yDataMean(1);
y(3,2) = yDataMean(end);
[y(2,2) bestk] = min(yDataMean);


disp(strcat("### INFO: Best optimal number of streams for: ", bar_graph_experiment_names(2), " is at k = ", num2str(bestk)));

errorplus(1,2) = yDataCI(1,1)./1000;
errorplus(3,2) = yDataCI(end,1);
errorplus(2,2) = yDataCI(bestk,1);


y = y./1000;
errorplus = errorplus./1000;
x=categorical({'Low Data Rate','High Data Rate'});
errorminus=errorplus;


ctr =[1.5429    3.3429    5.1429
    2.0571    3.8571    5.6571];


hBar = bar(y, 0.8);                                                     % Return ‘bar’ Handle
for k1 = 1:size(y,2)
    drawnow
    ctr(k1,:) = bsxfun(@plus, hBar(k1).XData, hBar(k1).XOffset');       % Note: ‘XOffset’ Is An Undocumented Feature, This Selects The ‘bar’ Centres
    ydt(k1,:) = hBar(k1).YData; 
end
for k1 = 1:size(y,2)
    hBar(k1).Visible = 'off';
end

ctr = ctr*1.8;                                                % Return ‘bar’ Handle
set(groot,'defaultAxesTickLabelInterpreter','latex');  
hold on;
leg = [];
leg(1) = errorbar(ctr(1,:), y(:,1)', errorplus(:,1)', errorminus(:,1)','Color',publicationColors(1,:),'MarkerSize',get(0,'defaultLineMarkerSize'),'MarkerFaceColor','auto','LineStyle','none');
leg(2) = errorbar(ctr(2,:), y(:,2)', errorplus(:,2)', errorminus(:,2)','Color',publicationColors(2,:),'MarkerSize',get(0,'defaultLineMarkerSize'),'MarkerFaceColor','auto','LineStyle','none');
ax = gca;
xlim([min(ctr(:))-0.5,max(ctr(:))+0.5]);
ax.XTick = [sum(ctr)/2];
ax.XTickLabel = [{"$k=1$"};{"$k=k_o$"};{"$k=N$"}];
ylabel({'Avg. AoI [s]'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
box on;

xlabel("Topology A",...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')

newBar = bar(sum(ctr)/2,y',0.8);
newBar(1).FaceColor = repmat(publicationColors(1,:),1,1);
newBar(2).FaceColor = repmat(publicationColors(2,:),1,1);
ylim([0,4.5]);


nexttile;
hold on;

experiments =  load_experiments(path_to_processed_results,bar_graph_experiment_names(3));
maxSeq = 0;
maxRng = 0;
numBBNodes = 0;
for experimentID = 1:size(experiments,2)
    
    if(isfield(experiments(experimentID).results,'rngID'))
        experiments(experimentID).results.rngID = str2double(experiments(experimentID).results.rngID);
    else
        experiments(experimentID).results.rngID = 1;
    end
    
    if(experiments(experimentID).results.numSeq>maxSeq)
        maxSeq=experiments(experimentID).results.numSeq;
    end
    if(experiments(experimentID).results.rngID>maxRng)
        maxRng = experiments(experimentID).results.rngID;
    end
    if(isfield(experiments(1).results,'rtxMean'))
        if(size(experiments(1).results.rtxMean,2)>numBBNodes)
            numBBNodes = size(experiments(1).results.rtxMean,2);
        end
    else
        numBBNodes=1;
    end
end

xData = repmat([1:maxSeq],maxRng,1);
yData = Inf(maxRng,maxSeq);

for experimentID = 1:size(experiments,2)
    if(isfield(experiments(experimentID).results,'avgAoI'))
        if(yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
            > experiments(experimentID).results.avgAoI)
            yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
        end
    end
end
% Squash Inf rows if missing data
yData(all(isinf(yData),2),:)=[];
% Only select first numRuns entries
if(size(yData,1)>numRuns)
    yData = yData(1:numRuns,:);
    xData = xData(1:numRuns,:);
end
global confidenceLevel;
yData(isinf(yData)) = NaN;
yDataCI = zeros(size(xData,2),2);
for xID = 1:size(xData,2)
   yDataCI(xID,:) =  calculate_ci(yData(:,xID),confidenceLevel);
end
yDataMean = mean(yData,1,'omitnan');

y(1,1) = yDataMean(1);
y(3,1) = yDataMean(end);
[y(2,1) bestk] = min(yDataMean);

disp(strcat("### INFO: Best optimal number of streams for: ", bar_graph_experiment_names(3), " is at k = ", num2str(bestk)));

errorplus(1,1) = yDataCI(1,1);
errorplus(3,1) = yDataCI(end,1);
errorplus(2,1) = yDataCI(bestk,1);

experiments =  load_experiments(path_to_processed_results,bar_graph_experiment_names(4));
maxSeq = 0;
maxRng = 0;
numBBNodes = 0;
for experimentID = 1:size(experiments,2)
    
    if(isfield(experiments(experimentID).results,'rngID'))
        experiments(experimentID).results.rngID = str2double(experiments(experimentID).results.rngID);
    else
        experiments(experimentID).results.rngID = 1;
    end
    
    if(experiments(experimentID).results.numSeq>maxSeq)
        maxSeq=experiments(experimentID).results.numSeq;
    end
    if(experiments(experimentID).results.rngID>maxRng)
        maxRng = experiments(experimentID).results.rngID;
    end
    if(isfield(experiments(1).results,'rtxMean'))
        if(size(experiments(1).results.rtxMean,2)>numBBNodes)
            numBBNodes = size(experiments(1).results.rtxMean,2);
        end
    else
        numBBNodes=1;
    end
end

xData = repmat([1:maxSeq],maxRng,1);
yData = Inf(maxRng,maxSeq);

for experimentID = 1:size(experiments,2)
    if(isfield(experiments(experimentID).results,'avgAoI'))
        if(yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq)...
            > experiments(experimentID).results.avgAoI)
            yData(experiments(experimentID).results.rngID,experiments(experimentID).results.numSeq) = experiments(experimentID).results.avgAoI;
        end
    end
end
% Squash Inf rows if missing data
yData(all(isinf(yData),2),:)=[];
% Only select first numRuns entries
if(size(yData,1)>numRuns)
    yData = yData(1:numRuns,:);
    xData = xData(1:numRuns,:);
end
global confidenceLevel;
yData(isinf(yData)) = NaN;
yDataCI = zeros(size(xData,2),2);
for xID = 1:size(xData,2)
   yDataCI(xID,:) =  calculate_ci(yData(:,xID),confidenceLevel);
end
yDataMean = mean(yData,1,'omitnan');

y(1,2) = yDataMean(1);
y(3,2) = yDataMean(end);
[y(2,2) bestk] = min(yDataMean);

disp(strcat("### INFO: Best optimal number of streams for: ", bar_graph_experiment_names(4), " is at k = ", num2str(bestk)));

errorplus(1,2) = yDataCI(1,1)./1000;
errorplus(3,2) = yDataCI(end,1);
errorplus(2,2) = yDataCI(bestk,1);


y = y./1000;
errorplus = errorplus./1000;
x=categorical({'Low Data Rate','High Data Rate'});

errorminus=errorplus;
hBar = bar(y, 0.8);                                                     % Return ‘bar’ Handle
for k1 = 1:size(y,2)
    drawnow
    ctr(k1,:) = bsxfun(@plus, hBar(k1).XData, hBar(k1).XOffset');       % Note: ‘XOffset’ Is An Undocumented Feature, This Selects The ‘bar’ Centres
    ydt(k1,:) = hBar(k1).YData; 
end
for k1 = 1:size(y,2)
    hBar(k1).Visible = 'off';
end

ctr = ctr*1.8;                                                % Return ‘bar’ Handle
set(groot,'defaultAxesTickLabelInterpreter','latex');  
hold on;
leg = [];
leg(1) = errorbar(ctr(1,:), y(:,1)', errorplus(:,1)', errorminus(:,1)','Color',publicationColors(1,:),'MarkerSize',get(0,'defaultLineMarkerSize'),'MarkerFaceColor','auto','LineStyle','none');
leg(2) = errorbar(ctr(2,:), y(:,2)', errorplus(:,2)', errorminus(:,2)','Color',publicationColors(2,:),'MarkerSize',get(0,'defaultLineMarkerSize'),'MarkerFaceColor','auto','LineStyle','none');


newBar = bar(sum(ctr)/2,y',0.8);
newBar(1).FaceColor = repmat(publicationColors(1,:),1,1);
newBar(2).FaceColor = repmat(publicationColors(2,:),1,1);
ylim([0,4.5]);

ax = gca;
xlim([min(ctr(:))-0.5,max(ctr(:))+0.5]);
ax.XTick = [sum(ctr)/2];
ax.XTickLabel = [{"$k=1$"};{"$k=k_{o}$"};{"$k=N$"}];
ylabel({'Avg. AoI [s]'},...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')
box on;

xlabel("Topology A",...
    'FontUnits','points',...
    'interpreter','latex',...
    'FontName','Times')

ylim([0,22]);



t.Position = [    0.1300    0.0800    0.7750    0.78];
lgd = legend(gca,leg,legendNames,'Location', 'Layout');
%legend boxoff;
lgd.NumColumns = 2;
lgd.ItemTokenSize = [10,10];
lgd.Position = [0.2252    0.96    0.5853    0.0706];
lgd.FontSize = 7.2;
lgd.Position = [0.2252    0.96    0.5853    0.0706];
xlabel("Topology B");
%% Export
if(export_figures)
    export_figure(fig,"bar_graph",path_to_export_figures);
end
