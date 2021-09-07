function [] = create_control_info(G,F,outputDir,numSeqs,comRange)
    
    %% Setup info struct
    G.Nodes.nType = ones(G.numnodes,1);

    G.Nodes.bbNeighbours = cell(G.numnodes,1);
    G.Nodes.fUp = zeros(G.numnodes,1);
    G.Nodes.fDown = cell(G.numnodes,1);
    G.Nodes.fTarget = cell(G.numnodes,1);
    G.Nodes.comRange = comRange.*ones(G.numnodes,1);

    % Define each mcds node as such
    G.Nodes.nType(F.Nodes.ID) = 2;
    
    % Allocate nodes based on distance in a round-robin fashion
    for nodeID = 1:G.numnodes
        candidateParents = intersect([neighbors(G,G.Nodes.ID(nodeID))],F.Nodes.ID);
        if(size(candidateParents,1) == 1)
            G.Nodes.fUp(nodeID) = candidateParents;
        else
            distance = [];
            for parentID = 1:size(candidateParents,1)
                distance(parentID) = sqrt( (G.Nodes.xPos(nodeID)-G.Nodes.xPos(candidateParents(parentID)))^2 + (G.Nodes.yPos(nodeID)-G.Nodes.yPos(candidateParents(parentID)))^2);
            end
            [~,i] = min(distance);
            G.Nodes.fUp(nodeID) = candidateParents(i);
        end
    end
    
    % Overwrite each mcds node as its own parent
    G.Nodes.fUp(F.Nodes.ID) = F.Nodes.ID;
    
    
    % Mirror fUp to each mcds down entry
    for bbNodeID = 1:F.numnodes
        G.Nodes.fDown(F.Nodes.ID(bbNodeID)) = {G.Nodes.ID([G.Nodes.fUp==F.Nodes.ID(bbNodeID)])'};
    end

    % Compute and add forwarding table to each mcds node
    nodeIDs = 1:F.numnodes;
    for bbNodeID = 1:F.numnodes
        tempHops = zeros(1,G.numnodes);
        for targetID = 1:G.numnodes
            targetBBNode = G.Nodes.fUp(targetID);
            targetBBIndex = nodeIDs((F.Nodes.ID == targetBBNode));

            % Find route to targetBB using the F subgraph nodes from bbNodeID
            if(targetBBIndex ~= bbNodeID)
                targetRoute = shortestpath(F,bbNodeID,targetBBIndex);
                tempHops(targetID) = F.Nodes.ID(targetRoute(2));    
            else
                tempHops(targetID) = targetBBNode;
            end
        end
        G.Nodes.bbNeighbours(F.Nodes.ID(bbNodeID)) = {F.Nodes.ID([neighbors(F,bbNodeID)])'};
        G.Nodes.fTarget(F.Nodes.ID(bbNodeID)) = {tempHops};
    end
    
    for seqID = 1:length(numSeqs)
        numSeq = numSeqs(seqID);
        saveName = strcat("Seq-",num2str(numSeq));
        
        G.Nodes.firstID = ones(G.numnodes,1);
        firstIDs = [];
        nextIDs = [];
        maxArray = [];
        while (length(maxArray) < G.numnodes)
           maxArray = [maxArray, 1:numSeq]; 
        end
        maxArray = sort(maxArray(1:G.numnodes));

        % translate seqID to nodeID
        for nodeID = 1:G.numnodes
            seqID = maxArray(nodeID);
            G.Nodes.firstID(nodeID) = find(maxArray==seqID,1);
        end

        for nodeID = 1:G.numnodes-1
            if(G.Nodes.firstID(nodeID) == G.Nodes.firstID(nodeID+1))
                G.Nodes.nextID(nodeID) = G.Nodes.ID(nodeID+1);
            else
                G.Nodes.nextID(nodeID) = G.Nodes.firstID(nodeID);
            end
        end
        G.Nodes.nextID(end) = G.Nodes.firstID(end);

        %% Create Control Info Files
        mkdir(strcat(outputDir, "/", saveName));
        for nodeID = 1:G.numnodes
            outStruct = [];
            outStruct.ID = G.Nodes.ID(nodeID);
            outStruct.maxID = G.numnodes;
            outStruct.xPos = G.Nodes.xPos(nodeID);
            outStruct.yPos = G.Nodes.yPos(nodeID);
            outStruct.type = G.Nodes.nType(nodeID);
            outStruct.firstInSeq = G.Nodes.firstID(nodeID);
            outStruct.nextInSeq = G.Nodes.nextID(nodeID);
            outStruct.comRange = G.Nodes.comRange(nodeID);

            outStruct.backBoneNeighbours = num2str([G.Nodes.bbNeighbours{nodeID}],'%i,');
            outStruct.backBoneNeighbours = outStruct.backBoneNeighbours(1:end-1);

            outStruct.forwardUp = G.Nodes.fUp(nodeID);

            outStruct.forwardDown = num2str([G.Nodes.fDown{nodeID}],'%i,');
            outStruct.forwardDown = outStruct.forwardDown(1:end-1);

            outStruct.backBoneHopDest = num2str([G.Nodes.ID'],'%i,');
            outStruct.backBoneHopDest = outStruct.backBoneHopDest(1:end-1);

            outStruct.backBoneHop = num2str([G.Nodes.fTarget{nodeID}],'%i,');
            outStruct.backBoneHop = outStruct.backBoneHop(1:end-1);

            writestruct(outStruct,...
                        strcat(outputDir, "/", saveName, "/Node-", num2str(nodeID),".xml"),...
                        'StructNodeName', "Node-Config");
        end
    end
end
