**The use of this software subjects you to our** [Terms Of Use](https://prrvchr.github.io/KiCad-BOM-CPL-Plugin/TermsOfUse_en)

## KiCad python plugin that generate BOM and CPL files for JCLPcb, LCSC and even any other supplier... such as Farnell.

### Install:

- Download the [plugin](https://github.com/prrvchr/KiCad-BOM-CPL-Plugin/archive/v0.0.1.zip)
- Unzip and put the plugin file `bom-cpl-plugin.py` in your KiCad working directory.
- In KiCad open Eeschema go to BOM (Generate Bill Of Materials) and add A New Plugin


### Use:

It is necessary for the operation to add 4 additional custom fields in Eeschema, which are:
- `Manufacturer`
- `PartNumber`
- `Supplier`
- `SupplierRef`

The required custom field `Supplier` must be set to one of the predefined supplier (LCSC or JLCPcb) or any other supplier name case insensitive.

it is also possible to add an optional `Quantity` field in order to be able to manage the quantities in Eeschema.
In the absence of this field, the default quantity is 1.

If necessary, a grouping is carried out on the quantities, making it possible to generate a single row for identical components in the BOM file.

If you know a little Python, the grouping and the order of the components in the BOM files is fully configurable ...

When launching the plugin (in Eeschema BOM) it will create as many BOM files as there are different `Supplier` encountered.

It also creates the CPL file for JLCPcb from one of the position files found in the working directory (ie: `your_project-all-pos.csv`, `your_project-top-pos.csv`, `your_project-bottom-pos.csv`).


### Predefined suppliers:

This plugin has been configured to work with LCSC, JLCPcb and also with many other suppliers ...

#### LCSC configuration:

CSV BOM file columns:
- Quantity
- Manufacture Part Number
- Manufacturer
- Description
- LCSC Part Number
- Package

CSV BOM componants are grouped by same PartNumber and SupplierRef

CSV BOM componants are order by: Manufacturer, PartNumber

No CPL file will be generated.

#### JLCPcb configuration:

CSV BOM file columns:
- Comment
- Designator
- Footprint
- LCSC Part #

No CSV grouping is performed, in the absence of a BOM Quantity column

CSV BOM componants are order by: SupplierRef

CPL file will be generated if a position file can be found.

#### Default configuration:

This configuration applies to all other suppliers (other than LCSC, JLCPcb)

CSV BOM file columns:
- Quantity
- Manufacture Part Number
- Manufacturer
- Description
- Supplier Part Number
- Package

CSV BOM componants are grouped by same PartNumber and SupplierRef

CSV BOM componants are order by: Manufacturer, PartNumber

No CPL file will be generated.


### Has been tested with:

* KiCad 5.1.5 - Ubuntu 18.04 - LxQt 0.11.1

### If errors occur:

In case of problem, :-(  
I encourage you to create an [issue](https://github.com/prrvchr/KiCad-BOM-CPL-Plugin/issues/new)
I will try to solve it :-)  
