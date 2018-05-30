--helper function
function getHeight(x, z)
	return api.ridgedNoise(x/2, z/2, 5, 0.01, 0.5) * 10 + 90
end

topBlock = 1 --grass
midBlock = 3 --dirt
fillerBlock = 2 --stone

function generateChunk(chunk_x, chunk_y, chunk_z)
	--print("generating chunk", chunk_x, chunk_y, chunk_z)
	local chunk = array3D(32)

	-- for x = 1, 32 do
	-- 	for z = 1, 32 do
	-- 		for y = 1, 32 do
	-- 			local actualHeight = y + (chunk_y * 32)
	-- 			if actualHeight < 32 then
	-- 				chunk[x][y][z] = fillerBlock
	-- 			end
	-- 		end
	-- 	end
	-- end

	-- return chunk


	local heights = array2D(34) --heightmap includes adjacent chunk blocks

	for x = 1, 34 do
		for z = 1, 34 do
			heights[x][z] = getHeight(x-1 + (chunk_x * 32), z-1 + (chunk_z * 32))
		end
	end

	for x = 1, 32 do
		for z = 1, 32 do
			local height = heights[x+1][z+1]

			local lineNegX =  normalize(vec3(x - 1, heights[x+2][z+1], z) - vec3(x, height, z));
			local linePosX =  normalize(vec3(x + 1, heights[x][z+1], z) - vec3(x, height, z));

			local lineNegY =  normalize(vec3(x, heights[x+1][z], z - 1) - vec3(x, height, z));
			local linePosY =  normalize(vec3(x, heights[x+1][z+2], z + 1) - vec3(x, height, z));

			local norm1 = cross(lineNegY, lineNegX);
			local norm2 = cross(linePosX, lineNegY);

			local norm3 = cross(linePosY, linePosX);
			local norm4 = cross(lineNegX, linePosY);

			local finalNorm = (norm1 + norm2 + norm3 + norm4) / vec3(4, 4, 4)
			local steep = dot(finalNorm, vec3(0, 1, 0)) < 0.7

			for y = 1, 32 do
				local actualHeight = y + (chunk_y * 32)

				if actualHeight < height then
					if steep then
						chunk[x][y][z] = fillerBlock
					else
						if actualHeight < height - 5 then
							chunk[x][y][z] = fillerBlock
						elseif actualHeight < height - 1 then
							chunk[x][y][z] = midBlock
						else
							chunk[x][y][z] = topBlock
						end
					end
				end
			end
		end
	end

	return chunk
end