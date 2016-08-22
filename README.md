CDM Service Framework
=====================

Welcome to the CDM Service Framework.

CDM Service Framework provides easy methods to implement CDM controllee and controller applications.
CDM Service Framework is based on AJTCL v16.04.


Folder Structure
----------------
<pre>
cdm_tcl
+---inc
|   \---interfaces
|       +---environment
|       +---input
|       \---operation
+---samples
|   +---common
|   +---inc
|   +---IntegratedControllee
|   +---Laundry
|   +---Test
|   \---VendorDefinedInterfaces
\---src
    +---CdmControllee
    \---interfaces
        +---environment
        +---input
        \---operation
</pre>

 * cdm_tcl/inc: header files
 * cdm_tcl/inc/interfaces: interface related header files
 * cdm_tcl/sampls/common: source files for device sample (AppHandler, PropertyStore)
 * cdm_tcl/sampls/inc: header files for device sample (AppHandler, PropertyStore)
 * cdm_tcl/samples/IntegratedControllee : integrated controllee
 * cdm_tcl/samples/Laundry : laundry sample
 * cdm_tcl/samples/Test: test sample for making other device sample
 * cdm_tcl/src/CdmControllee: controllee source files
 * cdm_tcl/src/interface: interface related source files

Building
--------
You can build CDM Service Framework using 'scons'.
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
        +---cdm
        \---cdm_tcl
</pre>

  * Build
<pre>
cd root-source-dir/core/ajtcl
scons TARG=linux WS=off VARIANT=debug


cd root-source-dir/services/base_tcl
scons TARG=linux WS=off VARIANT=debug

cd root-source-dir/services/cdm_tcl
scons TARG=linux WS=off VARIANT=debug
</pre>

  * Output
<pre>
cdm_tcl
+---build
\---dist
    +---bin
    +---include
    \---lib
</pre>

    - bin : TestSample
    - lib : libajtcl_cdm.a, libajtcl_cdm.so
    - inc : header files

Doxygen
-------
  * Make manual
<pre>
cd root-source-dir/services/cdm_tcl/docs
doxygen Doxygen_html
</pre>
