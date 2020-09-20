using GitVersionInfo;
using System.Reflection;
using System.Runtime.InteropServices;



// General Information about an assembly is controlled through the following 
// set of attributes. Change these attribute values to modify the information
// associated with an assembly.
[assembly: AssemblyTitle(VersionInfo.GIT_APPNAME)]
[assembly: AssemblyDescription("Disassemble Intel OMF85 library")]
#if DEBUG
[assembly: AssemblyConfiguration("debug")]
#else
[assembly: AssemblyConfiguration("")]
#endif
[assembly: AssemblyCompany("Mark Ogden")]
[assembly: AssemblyProduct(VersionInfo.GIT_APPNAME)]
[assembly: AssemblyCopyright("(C)" + VersionInfo.GIT_YEAR + " Mark Ogden")]
[assembly: AssemblyTrademark("")]
[assembly: AssemblyCulture("")]

[assembly: AssemblyInformationalVersion(VersionInfo.GIT_VERSION)]

// Setting ComVisible to false makes the types in this assembly not visible 
// to COM components.  If you need to access a type in this assembly from 
// COM, set the ComVisible attribute to true on that type.
[assembly: ComVisible(false)]


[assembly: Guid("4032347d-438d-4a3f-8d8b-fe8d9b39f5cc")]

[assembly: AssemblyVersion(VersionInfo.GIT_VERSION_RC)]
[assembly: AssemblyFileVersion(VersionInfo.GIT_VERSION_RC)]
