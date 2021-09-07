function [g,buildings,streets] = parse_osm_data(file_name,street_distance)

    raw_data = xml2struct(file_name);

    %% Parse Data
    [buildings,streets] = sift_osm_data(raw_data);

    %% Remove certain streets manually

    %% Connect Houses to Nearest Street Poly Point 
    buildings = connect_buildings(buildings,streets);

    %% Build Connectivtiy Matrix
    street_connectivity = connect_streets(streets,street_distance);
    street_connectivity = triu(street_connectivity);
    street_pos_x = vertcat(streets(:).x);
    street_pos_y = vertcat(streets(:).y);

    %% Create Graph
    g = graph(street_connectivity,'upper');

    %% Add Buildings
    for buildingID = 1:length(buildings)
        g = addedge(g,[g.numnodes+1],[(buildings(buildingID).streetID+(buildings(buildingID).streedIDPos-1)*length(streets))],...
            sqrt((buildings(buildingID).cX-streets((buildings(buildingID).streetID)).x((buildings(buildingID).streedIDPos)))^2+...
                 (buildings(buildingID).cY-streets((buildings(buildingID).streetID)).y((buildings(buildingID).streedIDPos)))^2));
    end
    g.Nodes.xPos = [street_pos_x(:,1); street_pos_x(:,2); [buildings.cX]'];
    g.Nodes.yPos = [street_pos_y(:,1); street_pos_y(:,2); [buildings.cY]'];
    g.Nodes.Type = [ones(1,length(street_connectivity)),1+ones(1,length([buildings.cX]))]';
    g.Nodes.id = [1:g.numnodes]';

    %% Generate Connected Graph / Update Information
    [g] = make_connected(g);
end

