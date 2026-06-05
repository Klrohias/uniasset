.PHONY: install serve build clean

# 安装依赖
install:
	uv sync

# 本地预览（开发模式）
serve:
	uv run mkdocs serve

# 构建静态站点
build:
	uv run mkdocs build

# 清理构建产物
clean:
	rm -rf site .venv
