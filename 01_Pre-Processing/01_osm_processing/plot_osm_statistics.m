function [] = plot_osm_statistics(fullGraph,feederMapping,debugPlots)

    centers = feederMapping.C;

    %% 1. Calculate the mean distance between LV/MV connection points
    % Calc
    % Generate distance matrix
    graphDistances = zeros(length(centers),length(centers));
    for centerID = 1:length(centers)
        graphDistances(centerID,:) = sqrt((centers(centerID,1)-centers(:,1)).^2 + (centers(centerID,2)-centers(:,2)).^2); 
    end
    % Generate all posssibile routes
    centerGraph = graph(graphDistances);
    % Find minimum spanning tree
    shortesNetwork = minspantree(centerGraph);
    % Calculate total network size in cable length
    networkLength = sum(shortesNetwork.Edges.Weight);
    % Normalize to MV/LV supply point
    normalizedNetworkLength = networkLength/length(centers);
    % Debug plot
    if(debugPlots)
        figure;
        hold on;
        p = plot(centerGraph,'EdgeLabel',centerGraph.Edges.Weight,'XData',centers(:,1),'YData',-centers(:,2));
        xlabel("xPos [km]");
        ylabel("yPos [km]");
        title("Minimum Length MV Network");
        highlight(p,shortesNetwork,'EdgeColor','r','LineWidth',3);
    end
    % Plot Results
    disp(strcat("### [1] The mean MV grid length per MV/LV supply points is: ",num2str(normalizedNetworkLength*1000)," meter."))

    %% 2. Calculate the mean distance for each LV customer

    normalizedNetworkLengths = zeros(1,length(centers));
    nodes = table2array(fullGraph.Nodes);
    buildingIDs = nodes(nodes(:,3) == 2,4);

    % Look at all feeder individually
    for centerID = 1:length(centers)
        % locate all nodes of the feeder
        nodeIDs = find(feederMapping.idx == centerID);
        % Calculate shortest path matrix
        shortesNetwork = shortestpathtree(fullGraph,buildingIDs(nodeIDs(1)),buildingIDs(nodeIDs));
        % Calculate total network size in cable length
        networkLength = median(shortesNetwork.Edges.Weight);
        % Normalize to MV/LV supply point
        normalizedNetworkLengths(centerID) = networkLength;%/length(nodeIDs);
    end
    disp(strcat("### [2] The mean LV grid length per LV supply points is: ",num2str(mean(normalizedNetworkLengths)*1000)," meter."))
    disp(strcat("### [2] The variance of the LV grid length per LV supply points is: ",num2str(var(normalizedNetworkLengths)*1000)," meter."))

end

