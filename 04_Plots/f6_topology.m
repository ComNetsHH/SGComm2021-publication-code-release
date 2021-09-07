%% Static Config
fig = figure('Units','points','Position',[0 0 12*21 6*21],'PaperPositionMode','auto');
colororder(publicationColors)

shrinkfactor = 0.55;

tiledlayout(1,1, 'Padding', 'none', 'TileSpacing', 'compact');    
nexttile   
hold on;

load('/simresults/sim/leonard/sg-flooding/XX_output_osm/osmWS.mat')
idx = feederMapping.idx;
C = feederMapping.C;

nodes = table2array(fullGraph.Nodes);
buildingIDs = nodes(nodes(:,3) == 2,4);
streetIDs = nodes(nodes(:,3) == 1,4);


cIDX=7;
xPositions = fullGraph.Nodes.xPos(buildingIDs([idx==cIDX]));
yPositions = -fullGraph.Nodes.yPos(buildingIDs([idx==cIDX]));
k = boundary(xPositions,yPositions,shrinkfactor);
fill(xPositions(k),yPositions(k),[0.90,0.90,0.90])
H = plot(xPositions(k),yPositions(k),'Color',publicationColors(2,:),'LineWidth',0.75);
cIDX=14;
xPositions = fullGraph.Nodes.xPos(buildingIDs([idx==cIDX]));
yPositions = -fullGraph.Nodes.yPos(buildingIDs([idx==cIDX]));
k = boundary(xPositions,yPositions,shrinkfactor);
fill(xPositions(k),yPositions(k),[0.90,0.90,0.90])
H = plot(xPositions(k),yPositions(k),'Color',publicationColors(2,:),'LineWidth',0.75);



for cIDX = 1:numel(unique(idx))
    xPositions = fullGraph.Nodes.xPos(buildingIDs([idx==cIDX]));
    yPositions = -fullGraph.Nodes.yPos(buildingIDs([idx==cIDX]));
    if(cIDX ~= 14 && cIDX~=7)
        k = boundary(xPositions,yPositions,shrinkfactor);
        H = plot(xPositions(k),yPositions(k),'Color','k','LineWidth',0.75);
    end
end

H = plot(fullGraph,'XData',fullGraph.Nodes.xPos,'YData',-fullGraph.Nodes.yPos,'MarkerSize',0.001,'LineWidth',0.2,'EdgeColor',[0.3,0.3,0.3]);

nodes = table2array(fullGraph.Nodes);
buildingIDs = nodes(nodes(:,3) == 2,4);
streetIDs = nodes(nodes(:,3) == 1,4);
highlight(H,buildingIDs,'MarkerSize',0.8,'NodeColor',publicationColors(3,:));

highlight(H,buildingIDs(feederMapping.idx==7),'MarkerSize',1,'NodeColor',publicationColors(2,:),'Marker','d');
highlight(H,buildingIDs(feederMapping.idx==14),'MarkerSize',1,'NodeColor',publicationColors(2,:),'Marker','d');
xticklabels({})
yticklabels({})
xticks({})
yticks({})
axis equal
box on
xlim([  661.2487  663.5042]);
ylim([1.0e+03 .*[   -3.7429   -3.7415]]);

% annotation(fig,'textarrow',[0.142857142857143 0.196428571428571],...
%     [0.113095238095238 0.220238095238095],'String',{'A'},'FontWeight','bold','HeadWidth',5,'HeadLength',5);
% annotation(fig,'textarrow',[0.800595238095238 0.666666666666667],...
%     [0.607142857142857 0.511904761904762],'String',{'B'},'LineWidth',0.5,...
%     'FontWeight','bold','HeadWidth',5,'HeadLength',5);

% Create textbox
annotation(fig,'textbox',...
    [0.543749999999999 0.616591742038329 0.023511904761905 0.0249750245284368],...
    'String','B',...
    'FontWeight','bold',...
    'FitBoxToText','off',...
    'EdgeColor','none');
annotation(fig,'textbox',...
    [0.147916666666667 0.228313353759942 0.023511904761905 0.0249750245284367],...
    'String',{'A'},...
    'FontWeight','bold',...
    'FitBoxToText','off',...
    'EdgeColor','none');

% 
% load('/simresults/sim/leonard/sg-flooding/XX_output_mcds/mcds7.mat')
% nexttile
% F = mcds_info.mcds_graph;
% G = mcds_info.full_graph;
% % Allocate nodes based on distance in a round-robin fashion
% for nodeID = 1:G.numnodes
%     candidateParents = intersect([neighbors(G,G.Nodes.ID(nodeID))],F.Nodes.ID);
%     if(size(candidateParents,1) == 1)
%         G.Nodes.fUp(nodeID) = candidateParents;
%     else
%         distance = [];
%         for parentID = 1:size(candidateParents,1)
%             distance(parentID) = sqrt( (G.Nodes.xPos(nodeID)-G.Nodes.xPos(candidateParents(parentID)))^2 + (G.Nodes.yPos(nodeID)-G.Nodes.yPos(candidateParents(parentID)))^2);
%         end
%         [~,i] = min(distance);
%         G.Nodes.fUp(nodeID) = candidateParents(i);
%     end
% end
% % Overwrite each mcds node as its own parent
% G.Nodes.fUp(F.Nodes.ID) = F.Nodes.ID;
% colors = hsv(length(F.Nodes.ID));
% H = plot(G,'XData',G.Nodes.xPos,'YData',-G.Nodes.yPos,'NodeLabel',{},'LineWidth',0.04,'EdgeColor','k');
% for fID = 1:length(F.Nodes.ID)
%     highlight(H,G.Nodes.ID(G.Nodes.fUp==F.Nodes.ID(fID)),'NodeColor',colors(fID,:),'MarkerSize',1.6);
% end
% 
% xticklabels({})
% yticklabels({})
% xticks({})
% yticks({})
% box on
% ax =  gca();
% 
% load('/simresults/sim/leonard/sg-flooding/XX_output_mcds/mcds8.mat')
% nexttile
% F = mcds_info.mcds_graph;
% G = mcds_info.full_graph;
% % Allocate nodes based on distance in a round-robin fashion
% for nodeID = 1:G.numnodes
%     candidateParents = intersect([neighbors(G,G.Nodes.ID(nodeID))],F.Nodes.ID);
%     if(size(candidateParents,1) == 1)
%         G.Nodes.fUp(nodeID) = candidateParents;
%     else
%         distance = [];
%         for parentID = 1:size(candidateParents,1)
%             distance(parentID) = sqrt( (G.Nodes.xPos(nodeID)-G.Nodes.xPos(candidateParents(parentID)))^2 + (G.Nodes.yPos(nodeID)-G.Nodes.yPos(candidateParents(parentID)))^2);
%         end
%         [~,i] = min(distance);
%         G.Nodes.fUp(nodeID) = candidateParents(i);
%     end
% end
% % Overwrite each mcds node as its own parent
% G.Nodes.fUp(F.Nodes.ID) = F.Nodes.ID;
% colors = hsv(length(F.Nodes.ID));
% H = plot(G,'XData',G.Nodes.xPos,'YData',-G.Nodes.yPos,'NodeLabel',{},'LineWidth',0.04,'EdgeColor','k');
% for fID = 1:length(F.Nodes.ID)
%     highlight(H,G.Nodes.ID(G.Nodes.fUp==F.Nodes.ID(fID)),'NodeColor',colors(fID,:),'MarkerSize',1.6);
% end
% 
% xticklabels({})
% yticklabels({})
% xticks({})
% yticks({})
% box on
% ax =  gca();
% 
% % ylabel({'TX/ETX'},...
% %     'FontUnits','points',...
% %     'interpreter','latex',...
% %     'FontName','Times')
% %  xlabel({'Parallel Streams'},...
% %     'FontUnits','points',...
% %     'interpreter','latex',...
% %     'FontName','Times')
% %        
% % 
% % %ylabel("Age of Information [ms]");
% % ylims = ylim;
% % 
% % lgd = legend([{["Dynamic RTX (N=3)"]},{["Static RTX (N=1)"]}],'Location', 'northoutside');
% % %legend boxoff;
% % lgd.NumColumns = size(aoi_over_seq_experiment_names,1);
% % lgd.ItemTokenSize = [10,10];
% box on

%% Export
if(export_figures)
    export_figure(fig,"topology",path_to_export_figures);
end
