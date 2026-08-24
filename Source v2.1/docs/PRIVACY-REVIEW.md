# Privacy and repository hygiene review

The public-review package was checked for common personal or machine-specific data before packaging.

The checked source/document/config files contain no intentionally included:

- personal names;
- email addresses;
- phone numbers;
- account identifiers;
- API keys, passwords, tokens, or private keys;
- Windows `C:\\Users\\...` paths;
- local Desktop/Documents/Downloads paths;
- `/home/...` user paths;
- `/mnt/data/...` working paths;
- runtime log files or crash dumps.

The repository is also scoped to the r7/v2.1 review target rather than later experimental revisions.

As with any public repository, review `git diff --cached` in GitHub Desktop before pushing newly added local files.
