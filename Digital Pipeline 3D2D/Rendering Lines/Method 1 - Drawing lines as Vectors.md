[[SOURCES#^78b2c4|SOURCE]] #Methods

1. Enable back-facing culling.
2. Create a view map from detecting and project all contour/silhouette lines of the mesh into the image plane.
3. Project the view-map cells back onto the mesh.
4. Build an adjacency-occlusion graph by:
	1. Making each constant-visibility region becomes a node.
	2. Undirected edges represent neighbouring regions on the mesh.
	3. Directed edges (occlusion arcs) represent one region visually covering another.
5. Detect visibility conflicts by cycling through the graph searching for:
	1. Self-occluding objects.
	2. Mutual occlusion cycles between objects.
6. - Select regions with the most occlusions, and gather related occluding regions that should be processed together.
7. Calculate geodesic distances over the mesh surface from occluding regions and find the boundary where these distance fields meet. Use curvature-weighted distances so cuts tend to follow natural features instead of visible contours.
8. Cut the mesh
9. Assign the foremost separated component to a layer, remove it from the graph and repeat conflict detection and cutting until all remaining graph components are acyclic.
10. Sort layers by depth
11. Convert each layer into vector shapes.
12. Output an editable layered vector document.
# Results
![[Pasted image 20260608183725.png]]
![[Pasted image 20260608183739.png]]