#Design 

- Method 3

|               |                                                         |                                                          |                   |
| ------------- | ------------------------------------------------------- | -------------------------------------------------------- | ----------------- |
| **Character** | **Icon for field**                                      | **Icon for method**                                      | **Visibility**    |
| `-`           | ![](https://plantuml.com/img/private-field.png)         | ![](https://plantuml.com/img/private-method.png)         | `private`         |
| `#`           | ![](https://plantuml.com/img/protected-field.png)       | ![](https://plantuml.com/img/protected-method.png)       | `protected`       |
| `~`           | ![](https://plantuml.com/img/package-private-field.png) | ![](https://plantuml.com/img/package-private-method.png) | `package private` |
| `+`           | ![](https://plantuml.com/img/public-field.png)          | ![](https://plantuml.com/img/public-method.png)          | `public`          |
# Overall Outline Extraction Pipeline

```plantuml
class PolygonMesh {  
+vertices  
+edges  
+triangles  
}  
  
class FeaturePathExtractor {  
+extractFeatures()  
}  
  
class SilhouetteDetector  
class SuggestiveContourDetector  
class CreaseEdgeDetector  
class BoundaryDetector  
  
PolygonMesh --> FeaturePathExtractor  
  
FeaturePathExtractor --> SilhouetteDetector  
FeaturePathExtractor --> SuggestiveContourDetector  
FeaturePathExtractor --> CreaseEdgeDetector  
FeaturePathExtractor --> BoundaryDetector
```
# Feature Extraction Hierarchy
```plantuml
abstract class FeatureLineDetector {  
+detect(mesh)  
}  
  
class SilhouetteDetector  
class SuggestiveContourDetector  
class CreaseEdgeDetector  
class BoundaryDetector  
  
FeatureLineDetector <|-- SilhouetteDetector  
FeatureLineDetector <|-- SuggestiveContourDetector  
FeatureLineDetector <|-- CreaseEdgeDetector  
FeatureLineDetector <|-- BoundaryDetector  
  
class FeatureSegment  
  
FeatureLineDetector --> "0..*" FeatureSegment : creates
```

# Hidden Line Removal
```plantuml
class FeatureSegment  
  
class HiddenSegmentRemover {  
+createIDReferenceImage()  
+renderIDBuffer()  
+compareDepth()  
+removeOccludedSegments()  
}  
  
class VisibleFeatureSegment  
  
FeatureSegment --> HiddenSegmentRemover  
HiddenSegmentRemover --> VisibleFeatureSegment
```

# GPU Visibility Pipeline
```plantuml
class PolygonMesh  
  
class GeometryShader {  
+generateSegments()  
}  
  
class ZBuffer {  
+depthTest()  
}  
  
class IDReferenceImage  
  
class HiddenSegmentFilter {  
+filter()  
}  
  
PolygonMesh --> GeometryShader  
GeometryShader --> ZBuffer  
ZBuffer --> IDReferenceImage  
IDReferenceImage --> HiddenSegmentFilter
```

# Linking Feature Segments
```plantuml
class VisibleFeatureSegment  
  
class FeaturePathLinker {  
+findAdjacentSegments()  
+mergeSegments()  
+buildContinuousPaths()  
}  
  
class FeaturePath  
  
VisibleFeatureSegment --> FeaturePathLinker  
FeaturePathLinker --> FeaturePath
```

# Complete Outline Generation System
```plantuml
class PolygonMesh  
  
class FeaturePathExtractor {  
+extractFeatures()  
}  
  
class SilhouetteDetector  
class SuggestiveContourDetector  
class CreaseEdgeDetector  
class BoundaryDetector  
  
class FeatureSegment  
  
class HiddenSegmentRemover {  
+removeOccludedSegments()  
}  
  
class VisibleFeatureSegment  
  
class FeaturePathLinker {  
+buildContinuousPaths()  
}  
  
class FeaturePath  
  
class PencilStrokeGenerator {  
+generateStroke()  
}  
  
PolygonMesh --> FeaturePathExtractor  
  
FeaturePathExtractor --> SilhouetteDetector  
FeaturePathExtractor --> SuggestiveContourDetector  
FeaturePathExtractor --> CreaseEdgeDetector  
FeaturePathExtractor --> BoundaryDetector  
  
SilhouetteDetector --> FeatureSegment  
SuggestiveContourDetector --> FeatureSegment  
CreaseEdgeDetector --> FeatureSegment  
BoundaryDetector --> FeatureSegment  
  
FeatureSegment --> HiddenSegmentRemover  
HiddenSegmentRemover --> VisibleFeatureSegment  
  
VisibleFeatureSegment --> FeaturePathLinker  
FeaturePathLinker --> FeaturePath  
  
FeaturePath --> PencilStrokeGenerator
```
