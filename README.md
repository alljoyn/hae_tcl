HAE Service Framework
=====================

Welcom to the HAE Service Framework.

HAE Service Framework provides easy methods to implement HAE controllee and controller applications.
HAE Service Framework is based on AJTCL v15.09.


Folder Structure
----------------
<pre>
tcl
+---inc
|   \---interface
|       \---hae
|           \---interfaces
|               +---environment
|               +---input
|               \---operation
+---samples
|   +---Test
|   +---common
|   \---inc
\---src
    +---HaeControllee
    \---interfaces
        +---environment
        +---input
        \---operation
</pre>

 * tcl/inc: header files
 * tcl/inc/interfaces: interface related header files
 * tcl/samples/Test: test sample for making other device sample
 * tcl/sampls/common: source files for device sample (AppHandler, PropertyStore)
 * tcl/sampls/inc: header files for device sample (AppHandler, PropertyStore)
 * tcl/src/HaeControllee: controllee source files
 * tcl/src/interface: interface related  source files

Building
--------
You can build HAE Service Framework using 'scons'.
Please refer to
<a name="AllJoyn build instructions" href="https:///allseenalliance.org/developers/develop/building">AllJoyn Build Instruction </a>
for more details on setting up your environment.

  * Folder structure
<pre>
root-source-dir
    +---core
    |   \---ajtcl
    \---services
        +---base_tcl
        \---hae
</pre>

  * Build
<pre>
cd root-source-dir/core/ajtcl
scons TARG=linux WS=off VARIANT=debug


cd root-source-dir/services/base_tcl
scons TARG=linux WS=off VARIANT=debug

cd root-source-dir/services/hae/tcl
scons TARG=linux WS=off VARIANT=debug
</pre>

  * Output
<pre>
tcl
+---build
\---dist
    +---bin
    +---include
    \---lib
</pre>

    - bin : TestSample
    - lib : libajtcl_hae.a, libajtcl_hae.so
    - inc : header files

Doxygen
-------
  * Make manual
<pre>
cd root-source-dir/services/hae/tcl/docs
doxygen Doxygen_html
</pre>
