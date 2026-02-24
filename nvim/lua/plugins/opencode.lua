return {
  "nickjvandyke/opencode.nvim",
  version = "*",
  dependencies = {
    {
      "folke/snacks.nvim",
      optional = true,
      opts = {
        terminal = {},
      },
    },
  },
  config = function()
    vim.o.autoread = true
    local default_width = 0.4

    vim.g.opencode_opts = {
      provider = {
        terminal = {
          split = "right",
          width = math.floor(vim.o.columns * default_width),
        },
        snacks = {
          auto_close = true,
          win = {
            position = "right",
            width = default_width,
            enter = false,
          },
        },
      },
    }

    local current_width = default_width

    local function find_opencode_win()
      for _, win in ipairs(vim.api.nvim_list_wins()) do
        local buf = vim.api.nvim_win_get_buf(win)
        local ft = vim.api.nvim_buf_get_option(buf, "filetype")
        if ft == "opencode_terminal" or ft == "snacks_terminal" then
          local name = vim.api.nvim_buf_get_name(buf)
          if name:match("opencode") then
            return win
          end
        end
      end
      return nil
    end

    local function resize_opencode(size)
      if not size then return end
      local width = size / 100
      current_width = width
      vim.g.opencode_opts.provider.snacks.win.width = width
      vim.g.opencode_opts.provider.terminal.width = math.floor(vim.o.columns * width)
      local win = find_opencode_win()
      if win then
        local new_width = math.floor(vim.o.columns * width)
        vim.api.nvim_win_set_width(win, new_width)
      end
    end

    local function smart_toggle()
      local win = find_opencode_win()
      if win then
        require("opencode").stop()
        vim.system({ "pkill", "-f", "opencode" }, { detach = true })
        vim.system({ "pkill", "-f", "gopls.*opencode" }, { detach = true })
        vim.system({ "pkill", "-f", "jdtls.*opencode" }, { detach = true })
      else
        require("opencode").toggle()
      end
    end

    local function opencode_command(opts)
      local args = opts.args
      if args and args ~= "" then
        local size = tonumber(args)
        if size then
          resize_opencode(size)
        end
      else
        smart_toggle()
      end
    end

    vim.api.nvim_create_user_command("Opencode", opencode_command, {
      nargs = "?",
      desc = "Toggle opencode",
      complete = function()
        return { "30", "40", "50", "60", "70" }
      end,
    })

    vim.cmd.cnoreabbrev("opencode", "Opencode")

    vim.api.nvim_create_user_command("OpencodeSize", function(opts)
      local size = tonumber(opts.args)
      if size then
        resize_opencode(size)
      end
    end, {
      nargs = "?",
      desc = "Resize opencode window",
      complete = function()
        return { "30", "40", "50", "60", "70" }
      end,
    })

    vim.keymap.set("n", "<leader>o", smart_toggle, { desc = "Toggle opencode" })
  end,
}
