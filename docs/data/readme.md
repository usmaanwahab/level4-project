## Data
Keep your data (e.g. from evaluations here)

* Keep a written description of the data, what is contained, and how it was captured in `data/readme.md`
* Record all raw data as an immutable store. **Never modify captured data.** 
    * Keep this under `data/raw`
    * This could be logs, questionnaire responses, computation results

* Write scripts to produced processed data from these (e.g. tidy dataframes, excel sheets, csv files, HDF5 files, sqlite databases)
* Write scripts that process these into results, visualisations, tables that you include in your project.

* You may need to remove the `data/` folder from version control if the data size is too large or you are bound by confidentiality.
* If you do so **make sure you have good backups**
