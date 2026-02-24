return {
  {
    "nvim-neo-tree/neo-tree.nvim",
    opts = {
      filesystem = {
        filtered_items = {
          visible = true,
          hide_dotfiles = false,
          hide_gitignored = false,
          hide_hidden = false,
          hide_by_name = {},
          hide_by_pattern = {},
          never_show = {},
        },
      },
      window = {
        mappings = {
          ["<CR>"] = "open_with_filter",
          ["H"] = "toggle_hidden",
        },
      },
      commands = {
        open_with_filter = function(state)
          local node = state.tree:get_node()
          require("neo-tree.sources.common.commands").clear_filter(state)
          if node.type == "file" then
            vim.cmd("edit " .. node.path)
          elseif node.type == "directory" then
            require("neo-tree.sources.common.commands").toggle_node(state)
          end
        end,
        toggle_hidden = function(state)
          state.filtered_items_visible = not state.filtered_items_visible
          require("neo-tree.sources.filesystem").toggle_show_hidden(state)
        end,
      },
    },
  },
  {
    "folke/snacks.nvim",
    opts = {
      explorer = { enabled = false },
      picker = {
        sources = {
          grep = {
            args = { "--no-ignore" },
          },
        },
      },
    },
  },
}
