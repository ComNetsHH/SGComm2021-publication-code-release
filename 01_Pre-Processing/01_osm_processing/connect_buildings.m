function [buildings] = connect_buildings(buildings,streets)
    for buildingID = 1:length(buildings)
        bestStreetID = -1;
        bestDistance = Inf;
        bestStreetIndex = -1;
        for streetID = 1:length(streets)
            distances = sqrt((-buildings(buildingID).cX+streets(streetID).x).^2+(-buildings(buildingID).cY+streets(streetID).y).^2);
            [minDistance,index] = min(distances);
            if(minDistance < bestDistance)
                bestDistance = minDistance;
                bestStreetID = streetID;
                bestStreetIndex = index;
            end
        end
        buildings(buildingID).streetID = bestStreetID;
        buildings(buildingID).streedIDPos = bestStreetIndex;
    end
end