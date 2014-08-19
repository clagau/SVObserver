There are two ways to view and build projects related to SVObserver.  RunRejectServer.sln contains the RunRejectServer runs alongside SVObserver when monitor lists are used.

1) The Single Solution Approach
  The traditional way to build SVObserver.exe is to use SVObserver.sln.  This solution contains the projects required to SVObserver.exe with one click.

2) The Multiple Solution Approach
  There are now alternate solutions that encompass smaller portions of the system.  The idea is that these may be easier to work with when a developer is tasked with changing a certain part of the system.  To build SVObserver from scratch using these solution files, they should be built in the following order:
a) Base.sln
b) Image Processing.sln
c) Hardware.sln
d) SVIM Tools.sln (optional)
e) Remote.sln
f) Vision Engine.sln