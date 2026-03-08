--- 12/02/2020 through 19/02/2020 ---
Questions for assistant:
  Suggestion for image format, prefer an industry standard  -> .ppm or .png or .jpeg
  Way to read Wavefront Object files with notepad/any other application program -> just unzip the file and open
  Find a way to define a working project structure without h2w lib -> for later, must focus on deadline
  Further explanation on the use of a Pixel data structure, and possible other screen/film related things -> Pixel not necessary

image format: .png of openexr
.gz unzippen en dan notepad
zoek op: stack vs heap

19/02/2020:
  Got the intersections working (or atleast to the point where a 2d image renders). Strange bug occurring
  where the sphere moves downwards if the z coordinate is incremented. Need to rethink the way ray directions, cameras
  and hitpoints are represented (in what coordinate systems?). Also: the image isn't taking the plane offset into
  account yet. The ray direction must change accordingly. Maybe ask help tomorrow. Light calculations are strange,
  must somehow be able to multiply and get a value between 0 and 255

20/02/2020:
  -- Questions --
  Do lines like intersection.object->material->kd add a lot of overhead? How to improve?
  Intersection.cpp line intersection = {smallest_t_so_far, n, object, ray}; what to do with object? if the original object gets deleted, will this too?
  What coordinate systems should the ray direction and/or eye be represented in?
  RGB arithmetic, how to retain relative values within 0 and 255
  Best practices for auxiliary functions like get_intersection_color(), within main.cpp or seperate .h/.cpp?
  -- Notes --
  Made progress on color representation. The sphere now reflects light originating from a point light. Must refine shading
  to support material properties. Basic arithmetic operators like + - / * of the rgb struct passed a reference of the original
  object as a result, which resulted in weird bugs. They now return a new copy. Still need to implement gamma correction,
  the correct calculation for a ray direction (i.e. deriving from the camera axes uvw) and new shapes like triangles etc...

21/02/2020:
  -- Notes --
  Wouldn't it be nicer if the origin of the ray was set to the intersection of the direction with the image plane,
  instead of the eye of the camera? That way no check for t >= min_t is needed in intersection calculations. (27/02/2020: this is definitely not true)

  Rays now 'shoot' correctly through a camera. Did some refactoring relating to intersection methods.
  The coordinate systems still behave strangely, putting eye at (0, 2.5, -10) gives a pink screen, while (1e-6, 2.5, -10)
  does not.

26/02/2020:
  BVH: First hierarchy of scene, then new hierarchy for triangle mesh OR entire hierarchy for scene? Difference in performance?

27/02/2020:
  Visibility & viewing ray intersections are working for all objects. Optimized triangle intersection. Now writing a
  wavefront obj parser. Next up is anti-aliasing or BVH.
  -- Questions --
  Seemingly left-handedness for eigen vector operations?
  Easy way to parallelize the outer loop?
  Clamping & color representation still not 100% clear
  Difference in complexity between nearest intersection for viewing ray and computing reflected light radiances
  -- Notes --
  Debugged the wavefront parser. Shading normals are now interpolated according to Phong interpolation.
  Anti-aliasing makes seemingly no difference. Can test this by plotting the color values before and after to see
  if the curve smooths because of super sampling.
  BVH representation -> Composite pattern (some papers say this is slow because of virtual function calls)
  2D/3D transformations -> Visitor pattern

28/02/2020:
  On the use of design patterns:
  BVH construction might be best done with a Builder pattern, however should look into other creational patterns to
  assess whether this gives the most utility overall. Why? Because BVH construction can be done in several ways,
  and it might be useful to make the code reusable and adaptable to several different construction algorithms.
  Also, transformations must also work for any type of light source. Thinking about whether or not it would be
  prudent to make a more general abstract class which Object and Light inherit from, and let transformations call
  on that.
  --> after reading some papers, some of these things are actually used. Object and Light usually inherit from a more
      abstract class Primitive. Acceleration structures often have Builders. Composite pattern for BVH representation is
      often claimed to be inefficient because of the way virtual function calls work. Haven't found any data to back up
      these claims however. Function templates might solve this. Does this mean for loops over polymorphic vectors are
      inefficient as well?

04/03/2020:
  Wrote a draft for BVH definition, intersection and construction. Recursive definition
  is giving problems, however the intersection code is very compact. Construction is done using
  a builder and a recursive method. This has a strange problem where defining BVH bvh at the start does not
  work because you need to construct either a leaf or a node, which have two different constructors.
  Something strange is going on with the intersection method in BVH and the constructor for BVH.
  Also, sometimes the initializer syntax doesn't work (for example when initializing Vector3ds in Sphere.
  Starting with draft for AreaLight today, tomorrow try to fix the BVH bugs with help from assistant.

08/04/2020:
  A month has gone by, and a lot has happened. The BVH implementation got some slight changes. It does not use
  the builder pattern. It uses a spatial medium split on alternating axes. It also keeps track of on what axis was
  split, in order to do a small optimization in the intersection method. There's a lot to be done on the BVH,
  but that is not in the scope of the official course. Future changes and features include SAH, creating a BVH out of
  the entire scene and improving the build method.

  AreaLight implementation took a while. The visibility function was hard to get down correctly. Geometric factors and
  pdfs were often misplaced. It is fully functional now, correctly following the mathematical formulation. This also
  holds for indirect lighting computation. After repeatedly refactoring the outgoing radiance function, a sufficiently
  clean implementation was made. This made adding specular reflection remarkably easy. Some design decisions must be
  discussed. These are the following:
    - Coupling diffuse/specular reflection computation
        (+): Very little code is added/duplicated. Caustics require no extra attention because it is a natural
             consequence of coupling diffuse & specular reflection.
        (-): Because they are coupled, changing how specular reflection works might influence how diffuse
             reflection works if proper care isn't made. It is also harder to control the recursion depth of diffuse
             and specular rays separately (is this really prudent though?).
        (?): Efficient CPU usage within a thread is already problematic. If in the future it's decided to compute
             both terms in separate threads, then the code must be adjusted. The question is, how likely is that situation?
    - Russian Roulette for path termination
        (+): Mathematically unbiased. Can save a lot of computation time when used properly. The research
             recommends this technique.
        (-): Choosing the value of alpha (chance to stop the recursion) is very tricky. The 'best' value varies from
             scene to scene. If done improperly, it can lead to much more computation time than is needed.
        (!): As of right now, the probability for termination is calculated as a function of the average
             reflection coefficient (across the three wavelengths rgb) and a special depth termination function.
             To expand on the latter, the function is designed to 'favor' lower recursion depths and punish depths
             close to a certain limit chosen by the developer. It cuts off at a value of 1/3 in order to avoid
             exploding the estimator value with a large exponentially growing correction factor. Example: a pixel is
             at depth 19, close to the limit of 20. A termination probability of 0.95 results in a bounce. This means the
             estimator is multiplied by a value of 1/0.05 = 50! The estimator at depth 0 is already multiplied by 18
             other factors, with the last few most likely being > 10. An explosion of radiance like this makes some
             pixels extremely bright, resulting in an overall darker picture. Proper research is needed to find out
             a good implementation for this idea.

  What's next:
    - Writing a draft for the scientific report on the convergence of Monte Carlo sampling applied to direct/hybrid/path
      tracing.
    - Transparent materials (with caustics)

