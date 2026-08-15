# Git Workflow #

Low-frequency reference for branching, committing, and releasing RatSlap.
Summarised from `docs/DEV_WORKFLOW.md`.

## Branch Model

- `main` is always the current stable release.
- `develop` is the root of new activity (feature work branches off it).
  - This fork currently only has `main`; `develop` is not present locally.
    When starting feature work, create it from `main` or upstream.

## Branch Naming

| Type    | Pattern             | Example                     |
|---------|---------------------|-----------------------------|
| Feature | `ft.<feature>`      | `ft.kitchen-sink-option`    |
| Hotfix  | `hf.<hotfix>`       | `hf.teflon-tape`            |
| Release | `rl.<release>`      | `rl.1.2.3`                  |

## Commits

Commits are GPG-signed (`--gpg-sign`) upstream; follow this if possible.

## Pull Requests

PRs/MRs target the upstream `develop` branch.

## Release Process (Summary)

1. Branch `rl.<version>` from `develop`.
2. Update documentation (README version history, etc.).
3. Commit with `--gpg-sign`, tag with `-s` (signed tag).
4. Merge into `develop`, push tags to all remotes.
5. Fast-forward merge tag into `main`.
6. `make distclean && make dist` to produce signed tarball.
7. Upload tarball + `.asc` to GitHub/GitLab releases page.

See `docs/DEV_WORKFLOW.md` for the full step-by-step procedure.