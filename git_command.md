# Git 与 GitHub 多人协作开发指南

> 适用场景：已经在 GitHub
> 上创建了一个仓库，团队成员需要共同开发、提交代码和同步修改。\
> 本文以命令行操作为主，Windows、macOS 和 Linux 均适用；Windows 可在
> PowerShell、Windows Terminal 或 VS Code 终端中执行命令。

## 目录

-   [一、Git、GitHub 与基本概念](#一gitgithub-与基本概念)
-   [二、初次使用 Git：安装与配置](#二初次使用-git安装与配置)
-   [三、获取 GitHub 仓库](#三获取-github-仓库)
-   [四、推荐的多人协作流程](#四推荐的多人协作流程)
-   [五、开发过程中常用命令](#五开发过程中常用命令)
-   [六、分支管理](#六分支管理)
-   [七、处理冲突](#七处理冲突)
-   [八、撤销修改与恢复操作](#八撤销修改与恢复操作)
-   [九、`.gitignore` 忽略文件](#九gitignore-忽略文件)
-   [十、团队协作建议](#十团队协作建议)
-   [十一、常见问题](#十一常见问题)
-   [附录：日常开发速查流程](#附录日常开发速查流程)

------------------------------------------------------------------------

## 一、Git、GitHub 与基本概念

-   **Git**：版本控制工具，用来记录文件的修改历史。
-   **GitHub**：托管 Git 仓库的网站，方便团队共享代码和协作。
-   **仓库（Repository）**：项目及其版本历史的集合。
-   **工作区（Working Tree）**：当前正在编辑的文件。
-   **暂存区（Staging Area）**：通过 `git add` 选中、准备提交的修改。
-   **提交（Commit）**：为一组修改创建一个本地版本记录。
-   **分支（Branch）**：一条独立的开发线，适合多人同时开发不同功能。
-   **远程仓库（Remote）**：托管在 GitHub 等平台上的仓库。通常默认名称为
    `origin`。
-   **合并冲突（Conflict）**：不同修改无法由 Git
    自动合并，需要人工确认。

最重要的区别：

``` text
编辑文件 → git add → git commit → git push
 工作区      暂存区      本地仓库      GitHub 远程仓库
```

`git add` 不会上传代码；`git commit` 只会在本地创建提交；`git push`
才会把提交推送到远程仓库。

## 二、初次使用 Git：安装与配置

### 2.1 安装 Git

先检查是否已经安装：

``` bash
git --version
```

如果提示找不到命令，需要先安装 Git：

-   **Windows**：访问 [Git for
    Windows](https://git-scm.com/download/win) 下载并安装。

-   **macOS**：可以安装 Xcode Command Line Tools，或访问 [Git
    官网](https://git-scm.com/download/mac)。

-   **Ubuntu / Debian**：

    ``` bash
    sudo apt update
    sudo apt install git
    ```

安装后重新打开终端，再运行 `git --version` 确认安装成功。

### 2.2 设置提交者信息

Git
会把用户名和邮箱写入每次提交的作者信息。建议使用团队约定的名称，以及你希望显示在提交记录中的邮箱。

``` bash
git config --global user.name "你的名字"
git config --global user.email "你的邮箱"
```

例如：

``` bash
git config --global user.name "Zhang San"
git config --global user.email "zhangsan@example.com"
```

以上邮箱仅为格式示例，请替换成自己的邮箱。若希望 GitHub
将提交关联到自己的账户，可使用 GitHub 账户已验证的邮箱

查看配置：

``` bash
git config --global --list
git config user.name
git config user.email
```

说明：

-   `--global` 表示对当前操作系统用户下的所有 Git 仓库生效。
-   如果只想为当前仓库单独设置身份，先进入该仓库目录，再执行不带
    `--global` 的配置命令。
-   `user.name` 和 `user.email` 是提交记录的作者信息，**不是** GitHub
    登录凭据。

### 2.3 选择 GitHub 连接方式：HTTPS 或 SSH

克隆仓库、推送代码时，需要通过 GitHub 身份验证。常见方式有 HTTPS 和
SSH，选一种即可,正常开发环境下推荐HTTPS。

#### 方式 A：HTTPS

克隆地址通常类似：

``` bash
git clone https://github.com/组织名/仓库名.git
```

在需要身份验证时，按照 GitHub 提示使用浏览器登录、Git Credential Manager
或受支持的凭据方式。GitHub 不支持把账户登录密码直接当作 Git HTTPS
密码使用。

不要把个人访问令牌（PAT）写进代码、提交记录或公开文档。

#### 方式 B：SSH

1.  检查是否已有 SSH 密钥：

    ``` bash
    ls ~/.ssh
    ```

    如果看到类似 `id_ed25519` 和 `id_ed25519.pub`
    的文件，可能已经有密钥。不要覆盖不确定用途的现有密钥。

2.  如果没有可用密钥，可以创建一对：

    ``` bash
    ssh-keygen -t ed25519 -C "你的邮箱"
    ```

    按提示选择保存位置，并按需设置密钥口令。

3.  显示公钥：

    ``` bash
    cat ~/.ssh/id_ed25519.pub
    ```

    Windows PowerShell 也可使用：

    ``` powershell
    Get-Content $HOME\.ssh\id_ed25519.pub
    ```

4.  复制**公钥**内容，在 GitHub 的 **Settings → SSH and GPG keys → New
    SSH key** 中添加。

5.  测试连接：

    ``` bash
    ssh -T git@github.com
    ```

    首次连接时，确认主机指纹来自 GitHub
    官方公布的信息后再接受。私钥文件（通常没有 `.pub`
    后缀）必须保密，绝不能上传到 GitHub 或发给他人。

SSH 仓库地址通常类似：

``` bash
git clone git@github.com:组织名/仓库名.git
```

## 三、获取 GitHub 仓库

### 3.1 克隆已有仓库（推荐）

如果仓库已经在 GitHub 上创建，通常直接克隆：

``` bash
git clone https://github.com/组织名/仓库名.git
cd 仓库名
```

如果团队采用 SSH，将第一条命令换成 SSH 地址即可。

克隆会下载项目文件及相关 Git 历史。一般不需要再对这个目录执行
`git init`。

### 3.2 检查仓库状态与远程地址

``` bash
git status
git branch
git remote -v
```

-  `git status`查看文件当前的修改状态
-  `git branch`查看当前分支
-  `git remote -v`查看远程仓库

通常可以看到当前分支为 `main`，远程仓库名为 `origin`。

## 四、推荐的多人协作流程

### 4.1 不建议所有人直接开发在 `main`

建议采用以下分工：

-   `main`：保持相对稳定，存放经过检查、可以集成的代码。
-   `feature/功能名`：开发新功能。
-   `fix/问题名`：修复缺陷。
-   通过 **Pull Request（PR）** 将功能分支合并到
    `main`，由队友进行代码审查。 (PR通常在github仓库页面上菜单栏)

示例分支名：

``` text
feature/login
feature/camera-inference
fix/employee-id-check
docs/git-guide
```

如果仓库启用了分支保护规则，请遵循仓库要求，例如必须通过
PR、代码审查或自动化测试后才能合并。

### 4.2 每次开始开发前

``` bash
git switch main
git pull origin main
git switch -c feature/功能名
```
-  `git switch` 切换分支  `-c`参数为切换并创建新分支

这会先更新本地
`main`，再创建并切换到新功能分支。分支名请换成实际功能名称。

如果你已经在自己的功能分支上继续开发，通常先同步远程 `main`
的最新变化，再按团队约定合并或变基；不要在未了解项目规则时随意执行
`rebase`。

### 4.3 开发过程中提交修改

``` bash
git status
git diff
git add 文件名
git commit -m "feat: 添加登录功能"
```

也可以一次暂存当前目录下的修改：

``` bash
git add .
```

提交前建议检查 `git status` 和
`git diff --staged`，确认没有把临时文件、密钥、个人配置或不相关修改一起提交。

`git commit`是给当前准备提交的修改一个描述

### 4.4 推送功能分支

第一次推送这个新分支时：

``` bash
git push -u origin feature/功能名
```

`-u` 会建立本地分支与远程分支的跟踪关系。之后在该分支上通常只需要：

``` bash
git push
```

### 4.5 创建 Pull Request

1.  打开 GitHub 仓库页面。
2.  选择刚刚推送的功能分支，创建 Pull Request。
3.  说明本次修改的目的、主要变化、测试方式，以及需要队友特别留意的地方。
4.  等待队友审查，并按反馈继续修改。
5.  检查通过后，按团队约定合并 PR。
6.  合并后更新本地 `main`，并删除已经完成的功能分支（如果不再需要）。

### 4.6 功能合并后清理本地分支

``` bash
git switch main
git pull origin main
git branch -d feature/功能名
```

如果远程功能分支还没有被删除，并且团队确认可以删除：

``` bash
git push origin --delete feature/功能名
```

不要删除其他人仍在使用的分支。

## 五、开发过程中常用命令

### 5.1 查看状态与修改

  命令                                用途
  ----------------------------------- ------------------------------------
  `git status`                        查看修改、暂存和未跟踪文件
  `git diff`                          查看尚未暂存的修改
  `git diff --staged`                 查看已暂存、下一次提交将包含的修改
  `git log --oneline`                 简洁查看提交历史
  `git log --oneline --graph --all`   以图形方式查看分支和提交历史
  `git show`                          查看最近一次提交的详细内容
  `git branch`                        查看本地分支
  `git branch -a`                     查看本地与远程跟踪分支
  `git remote -v`                     查看远程地址

### 5.2 暂存与提交

  命令                            用途
  ------------------------------- ------------------------------------
  `git add 文件名`                暂存指定文件
  `git add .`                     暂存当前目录下的修改
  `git add -p`                    逐块选择要暂存的修改
  `git commit -m "说明"`          创建提交
  `git commit --amend`            修改最近一次提交，可能改写提交历史
  `git restore --staged 文件名`   将文件移出暂存区，但保留工作区修改

提交信息尽量简短、清晰，描述这次提交做了什么。例如：

``` bash
git commit -m "feat: 添加摄像头推理功能"
git commit -m "fix: 修复员工ID重复检查"
git commit -m "docs: 更新项目安装说明"
```

### 5.3 同步远程仓库

  命令                          用途
  ----------------------------- ------------------------------------
  `git fetch origin`            获取远程更新，不自动合并到当前分支
  `git pull`                    获取并整合当前分支对应的远程更新
  `git pull origin main`        获取并整合远程 `main` 到当前分支
  `git push`                    推送当前已配置跟踪关系的分支
  `git push -u origin 分支名`   首次推送并设置上游分支

`git pull origin main` 会尝试把远程 `main`
整合进**当前所在分支**，因此执行前先确认当前分支：

``` bash
git branch --show-current
```

### 5.4 分支操作

  命令                           用途
  ------------------------------ --------------------------
  `git switch main`              切换到 `main`
  `git switch -c feature/name`   创建并切换到新分支
  `git branch`                   查看本地分支
  `git merge 分支名`             将指定分支合并到当前分支
  `git branch -d 分支名`         删除已合并的本地分支

### 5.5 临时保存未提交修改

``` bash
git stash
git stash list
git stash pop
```

-   `git stash`：临时保存符合条件的工作区和暂存区修改。
-   `git stash list`：查看暂存列表。
-   `git stash pop`：尝试恢复最近保存的修改，并移除对应 stash 记录。

如果恢复时发生冲突，需要按冲突处理流程解决。重要修改不要只保存在 stash
中；有需要时先创建提交或使用其他备份方式。

## 六、分支管理

### 6.1 合并分支

如果需要把 `feature/login` 合并进 `main`，先切换到接收修改的分支：

``` bash
git switch main
git pull origin main
git merge feature/login
```

如果没有冲突，Git 会完成合并，之后按团队约定推送：

``` bash
git push origin main
```

多人协作时，通常更推荐通过 GitHub Pull Request
完成合并，而不是每个人直接推送到 `main`。

### 6.2 查看分支与提交关系

``` bash
git branch -vv
git log --oneline --graph --decorate --all
```

### 6.3 `merge` 与 `rebase` 简单区别

-   `merge`：合并两条开发历史，可能产生一个合并提交。
-   `rebase`：把当前分支的提交重新放到另一个基点之后，可能改写提交历史。

团队初学阶段优先使用 PR 和常规 merge
流程即可。不要对已经共享、其他人基于其继续开发的公共分支随意 rebase
或强制推送。

## 七、处理冲突

冲突通常发生在不同分支修改了同一文件的相同或相邻位置时。

### 7.1 识别冲突

执行 `git pull` 或 `git merge` 后，如果 Git 提示冲突，先运行：

``` bash
git status
```

冲突文件中可能出现：

``` text
<<<<<<< HEAD
当前分支中的内容
=======
另一个分支中的内容
>>>>>>> feature/login
```

这些标记用于区分不同版本的内容，不能原样保留在最终代码中。

### 7.2 解决冲突步骤

1.  打开冲突文件，理解双方修改的目的。

2.  与相关开发者沟通，决定保留哪部分，或如何组合两边的修改。

3.  编辑文件，删除 `<<<<<<<`、`=======`、`>>>>>>>` 等冲突标记。

4.  运行相关程序或测试，确认结果正确。

5.  将解决后的文件标记为已解决：

    ``` bash
    git add 冲突文件名
    ```

6.  如果正在进行 merge，按 Git 提示完成合并提交：

    ``` bash
    git commit
    ```

7.  继续推送当前分支：

    ``` bash
    git push
    ```

### 7.3 取消合并

如果正在进行合并且尚未完成，可以尝试：

``` bash
git merge --abort
```

这会尝试恢复合并开始前的状态。执行前请检查并备份重要的未提交修改。

## 八、撤销修改与恢复操作

> **警告：撤销命令的影响不同。执行前先运行
> `git status`，不确定时不要使用带 `--hard` 的命令。**

  ------------------------------------------------------------------------------------
  命令                                作用
  ----------------------------------- ------------------------------------------------
  `git restore 文件名`                丢弃该文件尚未暂存的修改

  `git restore --staged 文件名`       取消暂存，不丢弃工作区修改

  `git revert HEAD`                   新建提交来撤销最近一次提交的变更

  `git reset --soft HEAD~1`           移除最近一次提交记录，保留修改在暂存区

  `git reset --mixed HEAD~1`          移除最近一次提交记录，保留修改在工作区；默认
                                      reset 模式

  `git reset --hard HEAD~1`           重置提交、暂存区和工作区，可能丢失未保存的修改
  ------------------------------------------------------------------------------------

### 常见情况

**情况 A：改了文件，但不想保留尚未暂存的修改**

``` bash
git restore 文件名
```

这会丢弃该文件未暂存的改动。重要修改先备份。

**情况 B：已经 `git add`，但还没有提交**

``` bash
git restore --staged 文件名
```

文件会退出暂存区，修改仍保留在工作区。

**情况 C：提交已经推送到公共分支，想撤销该提交**

通常优先使用：

``` bash
git revert 提交ID
git push
```

如果要撤销最近一次提交，可以使用
`git revert HEAD`。它会生成新的撤销提交，通常比改写公共历史更安全。

**情况 D：想修改最近一次尚未共享的提交**

``` bash
git commit --amend
```

如果该提交已经推送或被他人基于它继续开发，先与团队沟通，不要随意改写公共历史。

## 九、`.gitignore` 忽略文件

`.gitignore` 用来告诉 Git
哪些**尚未被跟踪**的文件或目录通常不需要提交，例如编译产物、缓存和本机配置。

C++ 项目示例（请根据实际构建工具调整）：

``` gitignore
# 编译生成文件
*.o
*.obj
*.exe
*.out

# 常见构建目录
build/
cmake-build-*/

# 编辑器和系统文件
.vscode/
.idea/
.DS_Store
Thumbs.db

# Python 缓存（混合项目可保留）
__pycache__/
*.py[cod]
.venv/
venv/

# 日志
*.log
```

注意：

-   `.gitignore` 规则需要根据项目实际情况调整。团队若需要共享 VS Code
    配置，应有选择地提交 `.vscode/` 中的公共配置，而不是一概忽略。
-   不要把密码、API
    密钥、访问令牌、私钥或包含敏感信息的配置文件提交到仓库。
-   `.gitignore` 通常不会自动取消已经被 Git
    跟踪的文件。如果文件已被跟踪，需要另行处理；不要直接删除团队需要的文件。
-   如果密钥已经提交到 Git 历史中，仅删除当前文件并加入 `.gitignore`
    并不足够。应尽快撤销或轮换该密钥，并按团队安全流程处理历史记录。

## 十、团队协作建议

1.  **不要所有人都直接向 `main` 推送。** 尽可能使用功能分支和 Pull
    Request。
2.  **开始开发前先同步。** 切换到 `main` 后执行
    `git pull origin main`，再创建自己的功能分支。
3.  **一次提交尽量只做一件事。** 便于代码审查，也更容易定位问题。
4.  **写清楚提交信息。** 例如
    `feat: 添加串口通信`、`fix: 修复数组越界`、`docs: 更新安装文档`。
5.  **提交前先检查。** 使用 `git status`、`git diff` 和
    `git diff --staged`。
6.  **不要提交构建产物和本地环境文件。** 根据项目建立合适的
    `.gitignore`。
7.  **不要提交任何凭据。** 包括密码、API Key、访问令牌和私钥。
8.  **发生冲突先沟通。** 尤其是涉及公共接口、配置文件或同一功能逻辑时。
9.  **不要随意强制推送。** `git push --force`
    可能覆盖远程分支历史；共享分支尤其要避免。
10. **团队约定保持一致。**
    统一默认分支、分支命名、提交信息格式、测试方式和 PR 审查要求。

## 十一、常见问题

### 11.1 `git push` 提示没有上游分支

首次推送新分支时执行：

``` bash
git push -u origin 当前分支名
```

### 11.2 提示 `Author identity unknown`

说明还没有设置提交者信息：

``` bash
git config --global user.name "你的名字"
git config --global user.email "你的邮箱"
```

### 11.3 提示 `Permission denied (publickey)`

这通常表示 SSH 身份验证未通过。检查：

-   GitHub 账户是否添加了对应公钥。
-   本机是否有对应的私钥。
-   当前仓库远程地址是否使用 SSH。
-   执行 `ssh -T git@github.com` 时是否通过验证。

如果不打算配置 SSH，也可以改用 GitHub 提供的 HTTPS
克隆地址并完成凭据验证。

### 11.4 提示没有权限推送

确认自己是否拥有仓库写入权限，或是否需要先 Fork 仓库并通过 Pull Request
提交修改。仅知道仓库地址并不代表拥有写入权限。

### 11.5 `git pull` 提示存在未提交的修改

先运行：

``` bash
git status
```

然后根据情况提交修改、使用 `git stash`
暂存，或谨慎处理文件。不要为了消除提示而直接执行会丢弃修改的命令。

### 11.6 本地与远程出现不同的提交历史

先确认当前分支、远程地址和仓库状态：

``` bash
git status
git branch -vv
git remote -v
git fetch origin
git log --oneline --graph --decorate --all
```

确认原因后再决定 merge、rebase
或其他处理方式。不要在不了解影响的情况下使用 `git push --force` 或
`git reset --hard`。

## 附录：日常开发速查流程

下面是一套适合初学者的功能分支工作流。请将示例分支名替换成实际功能名。

### 第一次参与项目

``` bash
git clone https://github.com/组织名/仓库名.git
cd 仓库名
git status
```

完成 Git 身份配置和 GitHub 身份验证后即可开始。

### 每次开始新功能

``` bash
git switch main
git pull origin main
git switch -c feature/功能名
```

### 修改并提交代码

``` bash
git status
git diff
git add .
git diff --staged
git commit -m "feat: 描述本次功能"
```

### 推送并发起 PR

``` bash
git push -u origin feature/功能名
```

之后在 GitHub 网页上创建 Pull Request，等待审查和合并。

### 功能合并后

``` bash
git switch main
git pull origin main
git branch -d feature/功能名
```

------------------------------------------------------------------------

**记住这条主线即可：**

``` text
克隆仓库 → 更新 main → 创建功能分支 → 修改代码
→ 检查差异 → add → commit → push → 创建 PR → 审查合并
```

如果仓库已经制定了自己的协作规范，以仓库的 `CONTRIBUTING.md`、README
和维护者要求为准。
