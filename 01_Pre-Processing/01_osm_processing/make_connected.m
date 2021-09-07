function [g] = make_connected(g)
    while length(unique(conncomp(g,'Type','weak')))>1
        sGraphs = conncomp(g,'Type','weak');
        
        % find biggest cluster
        [~,mGraph] = max(histcounts(sGraphs));
        
        % selected first non main subgraph
        if(mGraph == 1)
            sGraph = 2;
        else
            sGraph = 1;
        end
        
        % find nodes in sGraph
        nodes = table2array(g.Nodes);
        sNodes = (nodes(sGraphs == sGraph,:));
        mNodes = (nodes(sGraphs == mGraph,:));
        distances = inf(length(sNodes),length(mNodes));
        
        for sNodeID = 1:size(sNodes,1)
           for mNodeID = 1:size(mNodes,1)
              distances(sNodeID,mNodeID) = sqrt( (sNodes(sNodeID,1)-mNodes(mNodeID,1))^2 + (sNodes(sNodeID,2)-mNodes(mNodeID,2))^2);
           end
        end
        
        % find best connection
        minimum = min(min(distances));
        [a,b] = find(distances==minimum);
        
        % Make new edge
        g = addedge(g,sNodes(a(1),4),mNodes(b(1),4),minimum);
    end
end